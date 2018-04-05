int ga, gb=5;

volatile unsigned char buff1[3];
volatile unsigned char buff2[3]; 
volatile unsigned short int buff3[5][7];

typedef struct  {
  int field1;
  char field2;
}mystruct;

int siA;

void static_auto_local( void ) {
	int aiB;
  static int siC=3;
	int * apD;
	int aiE=4, aiF=5, aiG=6;

	siA = 2;
  aiB = siC + siA;
	apD = & aiB;
	(*apD)++;
	apD = &siC;
	(*apD) += 9;
	apD = &siA;
	apD = &aiE;
	apD = &aiF;
	apD = &aiG;
	(*apD)++;
	aiE+=7;
	*apD = aiE + aiF;
}

int control_structures(void){
  unsigned int i=0;
  unsigned int x=0, y=0, z=0;
    
  if (x){
     y++;
  } else {
    y--;
  }
  
  switch (x) {
  case 1:
    y += 3;
    break;
  case 31:
    y -= 5;
    break;
  default:
    y--;
    break;
  }

	do {
    x += 2;
  } while (x < 20);
		
  while (x<10) {
    x = x + 1;
  }
  
  for (i = 0; i < 10; i++){
    x += i;
  }
  
  return x+i+y+z;
}

unsigned int arrays(unsigned char n, unsigned char j) {
  volatile unsigned int i;
  
  i = buff2[0] + buff2[n];
  i += buff3[n][j];
	return i;
}

int fun3(int arg3_1, int arg3_2, int arg3_3, int arg3_4) {
	return arg3_1*arg3_2*arg3_3*arg3_4;
}

int fun2(int arg2_1, int arg2_2) {
	int i;
	for (i=0; i< arg2_2; i++) {
		if (i == arg2_1)
			return 3;
	}
	arg2_2 += fun3(arg2_1, 4, 5, 6);
	arg2_2 *= 3;
	return arg2_2;
}

int fun1(int arg1_1) {
	int n;
	n = fun2(arg1_1, 5);
	return (n*arg1_1);
}

int fun4(char a, int b, char c) {
	int x[8];
	x[0] = a * b;
	x[c] = b;
	return a+b+c;
}

int fun5(char a, int b, int c, char d) {
	return (int) a+b+c+d;
}
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
