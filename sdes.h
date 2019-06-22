/*Save space in the .c file, implicit declarations otherwise*/
void decrypt(int* cipher, int* decrypted, int* key1, int* key2);
void encrypt(int* binary, int* cipher, int* key1, int* key2);
void fileIO(char* filename, int* binary, int* cipher, int* key1, int* key2);
void decToBinary(int dec, int* binary, int bits);
void leftShift(int* binary, int length);
int binToDecimal(int* binary, int length, int num);
void generateKey(int input, int* key1, int* key2);
void F(int* subKey, int* L, int* R, int* merge1);
void initialPerm(int* binary, int* binaryL, int* binaryR);
void inverseIP(int* binary);
void expandPerm(int* R, int* expandR);
void XOR(int* binary, int* subKey, int bits);
void SBOX(int* expandR, int* sBoxPost);