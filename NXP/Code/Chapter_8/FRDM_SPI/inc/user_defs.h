#ifndef USER_DEFS_H
#define USER_DEFS_H

#define MASK(x) (1UL << (x))


typedef int32_t FX_18_14;
typedef int32_t FX_20_12;
typedef int32_t FX_22_10;
typedef int32_t FX_26_6; 
typedef int32_t FX_28_4; 

#define FXP (1)

#define DCM (0)

#define K_SCALE (1)

#define FL_TO_FX_18_14(a) ((FX_18_14) (a * (1<<14))) 
#define FL_TO_FX_20_12(a) ((FX_20_12) (a * (1<<12))) 
#define FL_TO_FX_28_4(a) ((FX_28_4) (a * (1<<4)))
#define FL_TO_FX_26_6(a) ((FX_26_6) (a * (1<<6))) 
#define FL_TO_FX_10_22(a) ((int32_t) (a *  4194304 ) )

// I/O pin assignments


// STATUS 
    
#define MD_ERROR             0x10U  /* error */
#define MD_ARGERROR          0x11U  /* error agrument input error */
#define MD_ERROR1            0x12U  /* error 1 */
#define MD_ERROR2            0x13U  /* error 2 */
#define MD_ERROR3            0x14U  /* error 3 */
#define MD_ERROR4            0x15U

#define MD_OK               0x00U /* register setting OK */
#define MD_SPT              0x01U /* IIC stop */
#define MD_NACK             0x02U /* IIC no ACK */
#define MD_BUSY1            0x03U /* busy 1 */
#define MD_BUSY2            0x04U /* busy 2 */

#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
