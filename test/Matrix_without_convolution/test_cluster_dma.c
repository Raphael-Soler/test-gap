/* PMSIS includes */
#include "pmsis.h"

int size= 64;   //Doit etre un multiple de 8 

struct cl_args_s
{
    uint32_t size;
    unsigned short *l1_buffer_matrice1;
    unsigned short *l1_buffer_matrice2;
    unsigned short *l1_buffer_temp_matrice_mult;
    unsigned short *l2_in_matrice1;
    unsigned short *l2_in_matrice2;
    unsigned short *l2_out_matrice1;
    unsigned short *l2_out_matrice2;  
};

PI_L2 static struct cl_args_s cl_arg;

/* Task executed by cluster cores. */
void cluster_dma(void *arg)
{
    struct cl_args_s *dma_args = (struct cl_args_s *) arg;
    unsigned short *l1_buffer_matrice1 = dma_args->l1_buffer_matrice1;
    unsigned short *l1_buffer_matrice2 = dma_args->l1_buffer_matrice2;
    unsigned short *l1_buffer_temp_matrice_mult = dma_args->l1_buffer_temp_matrice_mult; 
    unsigned short *l2_in_matrice1 = dma_args->l2_in_matrice1;
    unsigned short *l2_in_matrice2 = dma_args->l2_in_matrice2;
    unsigned short *l2_out_matrice1 = dma_args->l2_out_matrice1;
    unsigned short *l2_out_matrice2 = dma_args->l2_out_matrice2;
    uint32_t buffer_size = dma_args->size;
    uint32_t coreid = pi_core_id(), start = 0, end = 0;

    /* Core 0 of cluster initiates DMA transfer from L2 to L1. */
    if (!coreid)
    {
        printf("Core %d requesting DMA transfer from l2_in_matrice1 to l1_buffer_matrice1.\n", coreid);
        pi_cl_dma_copy_t copy;
        copy.dir = PI_CL_DMA_DIR_EXT2LOC;
        copy.merge = 0;
        copy.size = buffer_size*sizeof(unsigned short);
        copy.id = 0;
        copy.ext = (uint32_t) l2_in_matrice1;
        copy.loc = (uint32_t) l1_buffer_matrice1;

        pi_cl_dma_memcpy(&copy);
        pi_cl_dma_wait(&copy);
        printf("Core %d : Transfer done.\n", coreid);
        
        
        printf("Core %d requesting DMA transfer from l2_in_matrice2 to l1_buffer_matrice2.\n", coreid);
        pi_cl_dma_copy_t copy2;
        copy2.dir = PI_CL_DMA_DIR_EXT2LOC;
        copy2.merge = 0;
        copy2.size = buffer_size*sizeof(unsigned short);
        copy2.id = 0;
        copy2.ext = (uint32_t) l2_in_matrice2;
        copy2.loc = (uint32_t) l1_buffer_matrice2;

        pi_cl_dma_memcpy(&copy2);
        pi_cl_dma_wait(&copy2);
        printf("Core %d : Transfer done.\n", coreid);
    }

    start = (coreid * (buffer_size / pi_cl_cluster_nb_pe_cores()));
    end = (start - 1 + (buffer_size / pi_cl_cluster_nb_pe_cores()));

    /* Barrier synchronisation before starting to compute. */
    pi_cl_team_barrier(0);
    /* Each core computes on specific portion of buffer. */
    for (uint32_t i=start; i<=end; i++)
    {
       unsigned short result = 0;
       int row = i / size;
       int col = i % size;
       
       for (int k = 0; k < size; k++) 
       {
         result += l1_buffer_matrice1[row*size + k] * l1_buffer_matrice2[k*size + col];
       }
   	 
       l1_buffer_temp_matrice_mult[i] = result;   
    }
    
     pi_cl_team_barrier(0); //Barrier pour ne pas perturber le calcule si des coeurs sont en retard
    //Ecriture du résultat de la somme des 2 matrices dans matrice 1
    for (uint32_t i=start; i<=end; i++)
    {
        l1_buffer_matrice1[i] = (l1_buffer_matrice1[i] + l1_buffer_matrice2[i]);       
    }
    
    pi_cl_team_barrier(0);
     
    //Ecriture du résultat de la multiplication dans matrice 2
    for (uint32_t i=start; i<=end; i++)
    {  
        l1_buffer_matrice2[i] = l1_buffer_temp_matrice_mult[i];
    }
    /* Barrier synchronisation to wait for all cores. */
    pi_cl_team_barrier(0);

    /* Core 0 of cluster initiates DMA transfer from L1 to L2. */
    if (!coreid)
    {
        printf("Core %d requesting DMA transfer from l1_buffer_matrice1 to l2_out_matrice1.\n", coreid);
        pi_cl_dma_copy_t copy;
        copy.dir = PI_CL_DMA_DIR_LOC2EXT;
        copy.merge = 0;
        copy.size = buffer_size*sizeof(unsigned short);
        copy.id = 0;
        copy.ext = (uint32_t) l2_out_matrice1;
        copy.loc = (uint32_t) l1_buffer_matrice1;

        pi_cl_dma_memcpy(&copy);
        pi_cl_dma_wait(&copy);
        printf("Core %d : Transfer done.\n", coreid);
        
        
        printf("Core %d requesting DMA transfer from l1_buffer_matrice2 to l2_out_matrice2.\n", coreid);
        pi_cl_dma_copy_t copy2;
        copy2.dir = PI_CL_DMA_DIR_LOC2EXT;
        copy2.merge = 0;
        copy2.size = buffer_size*sizeof(unsigned short);
        copy2.id = 0;
        copy2.ext = (uint32_t) l2_out_matrice2;
        copy2.loc = (uint32_t) l1_buffer_matrice2;

        pi_cl_dma_memcpy(&copy2);
        pi_cl_dma_wait(&copy2);
        printf("Core %d : Transfer done.\n", coreid);
    }
}

/* Cluster main entry, executed by core 0. */
void master_entry(void *arg)
{
    printf("Cluster master core entry\n");
    /* Task dispatch to cluster cores. */
    pi_cl_team_fork(pi_cl_cluster_nb_pe_cores(), cluster_dma, arg);
    printf("Cluster master core exit\n");
}

void test_cluster_dma(void)
{
    printf("Entering main controller\n");
    uint32_t errors = 0;
    struct pi_device cluster_dev;
    struct pi_cluster_conf conf;

    uint32_t nb_cl_pe_cores = pi_cl_cluster_nb_pe_cores();
    uint32_t buffer_size = size*size;
    unsigned short *l2_in_matrice1 = pi_l2_malloc(buffer_size*sizeof(unsigned short));
    if (l2_in_matrice1 == NULL)
    {
        printf("l2_in_matrice1 buffer alloc failed !\n");
        pmsis_exit(-1);
    }
    
    unsigned short *l2_in_matrice2 = pi_l2_malloc(buffer_size*sizeof(unsigned short));
    if (l2_in_matrice2 == NULL)
    {
        printf("l2_in_matrice2 buffer alloc failed !\n");
        pmsis_exit(-1);
    }

    unsigned short *l2_out_matrice1 = pi_l2_malloc(buffer_size*sizeof(unsigned short));
    if (l2_out_matrice1 == NULL)
    {
        printf("l2_out_matrice1 buffer alloc failed !\n");
        pmsis_exit(-2);
    }
    
     unsigned short *l2_out_matrice2 = pi_l2_malloc(buffer_size*sizeof(unsigned short));
    if (l2_out_matrice2 == NULL)
    {
        printf("l2_out_matrice2 buffer alloc failed !\n");
        pmsis_exit(-2);
    }

    /* L2 Array Init. */
    for (uint32_t i=0; i<buffer_size; i++)
    {
        l2_in_matrice1[i] = (i+1)%5;
        l2_out_matrice1[i] = 0;
        l2_in_matrice2[i] = 1;
        l2_out_matrice2[i] = 0;
    }
    

    /* Init cluster configuration structure. */
    pi_cluster_conf_init(&conf);
    conf.id = 0;                /* Set cluster ID. */
    /* Configure & open cluster. */
    pi_open_from_conf(&cluster_dev, &conf);
    if (pi_cluster_open(&cluster_dev))
    {
        printf("Cluster open failed !\n");
        pmsis_exit(-3);
    }

    unsigned short *l1_buffer_matrice1 = pi_cl_l1_malloc(&cluster_dev, buffer_size*sizeof(unsigned short));
    if (l1_buffer_matrice1 == NULL)
    {
        printf("l1_buffer_matrice1 alloc failed !\n");
        pi_cluster_close(&cluster_dev);
        pmsis_exit(-4);
    }
    
    
    unsigned short *l1_buffer_matrice2 = pi_cl_l1_malloc(&cluster_dev, buffer_size*sizeof(unsigned short));
    if (l1_buffer_matrice2 == NULL)
    {
        printf("l1_buffer_matrice2 alloc failed !\n");
        pi_cluster_close(&cluster_dev);
        pmsis_exit(-4);
    }
    
    unsigned short *l1_buffer_temp_matrice_mult = pi_cl_l1_malloc(&cluster_dev, buffer_size*sizeof(unsigned short));
    if (l1_buffer_temp_matrice_mult == NULL)
    {
        printf("l1_buffer_temp_matrice_mult alloc failed !\n");
        pi_cluster_close(&cluster_dev);
        pmsis_exit(-4);
    }

    /* Init arg struct. */
    cl_arg.size = buffer_size;
    cl_arg.l1_buffer_matrice1 = l1_buffer_matrice1;
    cl_arg.l1_buffer_matrice2 = l1_buffer_matrice2;
    cl_arg.l1_buffer_temp_matrice_mult = l1_buffer_temp_matrice_mult;
    cl_arg.l2_in_matrice1 = l2_in_matrice1;
    cl_arg.l2_in_matrice2 = l2_in_matrice2;
    cl_arg.l2_out_matrice1 = l2_out_matrice1;
    cl_arg.l2_out_matrice2 = l2_out_matrice2;

    /* Prepare cluster task and send it to cluster. */
    struct pi_cluster_task *task = pi_l2_malloc(sizeof(struct pi_cluster_task));
    if (task == NULL)
    {
        printf("Cluster task alloc failed !\n");
        pi_cluster_close(&cluster_dev);
        pmsis_exit(-5);
    }
    
    pi_cluster_task(task, master_entry, &cl_arg);

    printf("Sending task.\n");
    #if defined(ASYNC)
    pi_task_t wait_task;
    pi_task_block(&wait_task);
    pi_cluster_send_task_to_cl_async(&cluster_dev, task, &wait_task);
    printf("Wait end of cluster task\n");
    pi_task_wait_on(&wait_task);
    printf("End of cluster task\n");
    #else
    pi_cluster_send_task_to_cl(&cluster_dev, task);
    #endif  /* ASYNC */

    pi_l2_free(task, sizeof(struct pi_cluster_task));
    pi_cl_l1_free(&cluster_dev, l1_buffer_matrice1, buffer_size*sizeof(unsigned short));
    pi_cl_l1_free(&cluster_dev, l1_buffer_matrice2, buffer_size*sizeof(unsigned short));

    printf("Close cluster after end of computation.\n");
    pi_cluster_close(&cluster_dev);

    /* Verification. */
    for (uint32_t i=0; i<buffer_size; i++)
    {
     if (l2_out_matrice1[i] != (unsigned short) (l2_in_matrice1[i] + l2_in_matrice2[i])) errors++;
    }
    
    int result=0;
    for (int i = 0; i < size; i++) 
    {
        for (int j = 0; j < size; j++) 
        {
            result = 0;
            for (int k = 0; k < size; k++) 
            {
                result += l2_in_matrice1[i*size + k] * l2_in_matrice2[k*size + j];
            } 
            if (result != l2_out_matrice2[i*size+j]) errors++;
        }
    }
        
        
    
/*
    //Affichage de la Matrice 1
    printf("Matrix 1 :\n");
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%d ", l2_in_matrice1[i * size + j]);
        }
        printf("\n");
    }
    printf("\n");
    
    
    //Affichage de la Matrice 2
    printf("Matrix 2 :\n");
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%d ", l2_in_matrice2[i * size + j]);
        }
        printf("\n");
    }
    printf("\n");
*/    
    
    //Affichage de la Matrice Somme
    printf("Matrix Summed :\n");
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%d ", l2_out_matrice1[i * size + j]);
        }
        printf("\n");
    }
    printf("\n");
    
    
    //Affichage de la Matrice Multipliée
    printf("Matrix multiplied :\n");
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%d ", l2_out_matrice2[i * size + j]);
        }
        printf("\n");
    }
    printf("\n");
    
 
        
    //Libération de la mémoire
    pi_l2_free(l2_out_matrice1, buffer_size*sizeof(unsigned short));
    pi_l2_free(l2_out_matrice2, buffer_size*sizeof(unsigned short));
    pi_l2_free(l2_in_matrice1, buffer_size*sizeof(unsigned short));
    pi_l2_free(l2_in_matrice2, buffer_size*sizeof(unsigned short));

    printf("\nCluster DMA done with %d error(s) !\n", errors);

    pmsis_exit(errors);
}




/* Program Entry. */
int main()
{
    printf("\n\nSize selected : %i\n", SIZEOFMATRIX);
    size=SIZEOFMATRIX;
    
    printf("\n\n\t *** PMSIS Cluster DMA Test ***\n\n");
    return pmsis_kickoff((void *) test_cluster_dma);
}