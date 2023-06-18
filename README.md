# test-gap

#*Installation procedure for the GreenWaves Technologies GAP processor development environment :*

I started by installing a Ubuntu 20.04 virtual machine from osboxes.org. I then update the local package lists from the software repositories :

![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/ec44818f-8cc7-46d5-83e9-f05878de3095)

I've installed the necessary dependencies :
 ![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/573df705-5fdb-4e4c-9126-a5e89d127e18)
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/1d6a08ec-d494-4d57-9363-1a3dfda84f8b)


As recommended, I have defined python3 as the default: 
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/929896b8-6f31-42ef-bd53-ff176dbad0cf)

Then I cloned the repository containing the toolchain:
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/0232ebfc-82f8-48d8-bd88-2d26b00b489d)


And I installed it:
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/70b1e0f6-ee0c-4b00-bc38-4db622f3dff5)

 
Then I cloned the SDK repository :
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/16419685-d161-4fde-8a55-3b9ed0e87dff)



 
And I've defined the target as GAPUINO_V3 :
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/6e12d401-de19-4ae1-9663-5bb7da5423dd)

I then installed all the python dependencies for the SDK and for the documentation:
 ![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/85b39393-028d-481a-982b-0c7bfa19c8b0)
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/e439c35f-edea-44de-8fba-9f4929bec8dc)

 
I then installed the entire SDK with the "make all" command.

I then tried to build the html page for the documentation but an error appeared indicating that sphinx-build could not be found :
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/fedb8134-8d6b-4ed6-a0e6-75be18870423)


I tried to install it via pip install but it already existed : 
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/5881466a-5975-4b69-801a-3c624fdf54e1)

And I was indeed able to find sphinx-build in the right place: 
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/cf599866-4a9f-48b1-b58a-27e3c333294e)

So I added the path to the system directory and ran the make command again :
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/b45d6878-e209-426f-a065-69a9845ad8dc)

But errors appeared indicating that files could not be found: 
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/270f284d-a0f7-404e-b42f-82bf81aa37e6)

In the example folder there was no gap9 folder, so I deleted the associated dependency in readme.rst as I would only need gap8.
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/8fcc63b0-73b4-40a1-b069-6c3f4cd46276)

The compilation removes the error and I do the same with the following problems: 
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/577d24cc-b91b-4c9d-ab26-4ea59611a4b6)

I am therefore deleting the link to gap9 in orange: 
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/f0a1c927-1e8a-4a5c-a657-26fbeeeb629c)

And I'm also removing the dependency on the Audio Framework since I won't need it and it won't compile properly.
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/172f196d-f44a-4e1b-b603-6a55d5309252)

Everything is compiled and we can continue :
 ![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/971b07ee-66e4-44d2-afe8-0de8a0532f4f)




**Running the Hello world Example:**

Now I'll run a first example displaying hello world on each of cores of the virtual processor:
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/a6b1af00-f334-419d-9f2f-7930c28af040)

Everything compiles and the code works :
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/3496020c-0b84-4cc7-a72b-fc49b94a3065)


I then created a bash script containing the make command. 
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/045fde55-0b15-4b6e-b722-047938720f33)
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/435b33ff-5bce-4776-a592-e230e6522a81)


I give the executions rights and run the script, it works.
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/a4037478-d2a9-4634-8d0a-b23f2bc4cf49)
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/48ed47df-2ace-4b57-90d4-bcef5f54742b)

 



**Running the matrix algorithm without the convolution:**

Using the cluster_dma example as a starting point, I was able to design an algorithm taking 2 matrices stored in memory L2 as input, these 2 matrices are sent to memory L1 and the 8-core cluster is responsible for multithreading the sum of the 2 matrices which it then stores in matrix 1 and the multiplication of the 2 matrices which is stored in matrix 2. The set is then returned to memory L2 and enters a verification stage in which the program checks that the cluster's calculations have been carried out correctly and returns an error if an element of the matrix does not have the correct result. The cluster is perfectly functional for matrices ranging from 8x8 to 96x96. Beyond 96x96, errors appear indicating poor cluster calculation, probably due to memory errors.
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/c182cf7c-279c-4f88-9f1c-cd113929930b)
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/4a409403-3c84-4392-8e25-6ed161dd40ba)
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/8dcada4c-7321-41d9-a135-ffdd77556121)

The bash script takes as an argument the size in width of the desired matrix via the command "./launch.sh -s 96" if no value is entered, the default value is 64.
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/3bf3fb00-82ac-4a1a-8aea-c52ecfc8a9ac)

The makefile retrieves this value via a "value" variable and links it in the .c file, which becomes usable via the SIZEOFMATRIX variable.
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/91bf00b9-4dc5-484d-ade7-4bf3e853d7c0)
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/244e5d8f-b670-4256-a9e1-13b5558a7584)



**Running the matrix algorithm with the convolution:**
The convolution of the matrix poses more difficulty, in fact I'm not sure that the results are calculated correctly, I've tried to adapt a function from the internet, the programme works but the results are probably incorrect. Also the convolution filter is made up of unsigned integers but it contains negative values (unsigned short filter[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1}) so there's probably an error here. The code accepts matrices between 8x8 and 32x32, beyond this value a memory error appears.
![image](https://github.com/Raphael-Soler/test-gap/assets/82362262/8e44f270-c16a-47d0-8ff5-dff14ad46b60)

 

