#ifndef DMA_CORE_H 
#define DMA_CORE_H 

/******************************************************************************
**
** FILE NAME    : dma-core.h
** PROJECT      : Danube
** MODULES      : Central DMA
**
** DATE         : 26 SEP 2005
** AUTHOR       : Wu Qi Ming
** DESCRIPTION  : Central DMA Driver Header
** COPYRIGHT    :       Copyright (c) 2006
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 26 SEP 2005  Wu Qi Ming      Initiate Version
** 25 OCT 2006  Xu Liang        Add GPL header.
*******************************************************************************/

#define DMA_OWN   1
#define CPU_OWN   0   
#define DMA_MAJOR 250

//Descriptors
#define DMA_DESC_OWN_CPU		0x0
#define DMA_DESC_OWN_DMA		0x80000000
#define DMA_DESC_CPT_SET		0x40000000
#define DMA_DESC_SOP_SET		0x20000000
#define DMA_DESC_EOP_SET		0x10000000


#define MISCFG_MASK    0x40
#define RDERR_MASK     0x20
#define CHOFF_MASK     0x10
#define DESCPT_MASK    0x8 
#define DUR_MASK       0x4 
#define EOP_MASK       0x2


#define DMA_DROP_MASK  1<<31

#define DANUBE_DMA_RX -1
#define DANUBE_DMA_TX 1


typedef struct dma_chan_map{
   char    dev_name[15];
   enum attr_t  dir;  
   int     pri; 
   int     irq;
   int     rel_chan_no;
}_dma_chan_map; 
 
 
 
#endif

