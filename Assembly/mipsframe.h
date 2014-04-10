/*
 * CS5161 Assignment 5
 * 
 * Ming Zhou 4465225
 * 
 * Activation Record Module - MIPS architecture
 */

struct F_frame_ {
	F_accessList formals;
	int locals;
	int offset;
	Temp_label begin_label;
};

struct F_access_ {
	enum {inFrame, inReg} kind;
	union {
		int offset;
		Temp_temp reg;
	} u;
};

