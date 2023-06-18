# User Test
#------------------------------------

VALUE = "64"

APP              = test
APP_SRCS        += test_cluster_dma.c
APP_INC         +=
APP_CFLAGS      += -D SIZEOFMATRIX=$(VALUE)

ifeq ($(ASYNC), 1)
APP_CFLAGS 	+= -DASYNC
endif

include $(GAP_SDK_HOME)/utils/rules/pmsis_rules.mk


