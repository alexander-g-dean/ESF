#define CLEAR_FIELD(REG, FLD) 				((REG) &= ~(FLD))
#define SET_FIELD(REG, FLD) 					((REG) |= (FLD))
// For pre-aligned value AL_VAL
#define MODIFY_FIELD_AL(REG, FLD, AL_VAL) ((REG) = ((REG) & ~(FLD)) | AL_VAL)
// For unaligned value VAL
// Start Listing Modify_Field
#define MODIFY_FIELD(reg, field, value) \
((reg) = ((reg) & ~(field ## _Msk)) | \
(((uint32_t)(value) << field ## _Pos) & field ## _Msk))
// End Listing Modify_Field
