/***************************
* FILE: sdes.c             *
* AUTHOR: Thomas Di Pietro *
* DATE: 17/04/2019         *
*                          *
* PURPOSE: Demonstrates    *
* the use S-DES encryption *
* and decryption           *      
***************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdes.h"

int main(int argc, char* argv[])
{

    if (argc != 2)
    {
        printf("Error: command-line parameters wrong.\n");
        printf("       e.g. ./sdes filename\n");
    }
    else
    {
        /*Init of binary arrays for S-DES*/
        int binary[8];
        int cipher[8];
        int key1[8];
        int key2[8];
        int input = -1;

        printf("\nS-DES:");
        /*10-bit binary has 0-1023 range in decimal
        therefore user input must be in range*/
        while (input < 0 || input > 1023)
        {
            printf("\nInput a number for 10-bit Key (0-1023):> ");
            scanf("%d", &input);
        }
        /*Generate the subkeys*/
        generateKey(input, key1, key2);
        fileIO(argv[1], binary, cipher, key1, key2);
    }
    return 0;
}

void fileIO(char* filename, int* binary, int* cipher, int* key1, int* key2)
{
    int i, j;
    FILE* input = fopen(filename, "r");
    char line[500];
    char decText[500]; /*per line*/
    char enText[500];
    int decrypted[8];
    char choice;

    printf("\nPRINT OUT:\nPlain text (p)\nEncrypted text (e)\nEncrypted text binary (b)\nDecrypted text (d)\nExit (z)\n:> ");
    scanf(" %c", &choice);

    if (input == NULL) /*if file doesn't exist*/
    {
        perror("Error opening file");
    }
    else
    {
        /*3 Levels, String -> Char -> 8-bit Binary*/
        /*each line*/
        while(fgets(line, 500, input) != NULL)
        {
            /*each char*/
            for (i = 0; i < strlen(line); i++)
            {
                decToBinary(line[i], binary, 8);

                encrypt(binary, cipher, key1, key2);
                if (choice == 'b')
                {
                    for (j = 0; j < 8; j++)
                    {
                        printf("%d", cipher[j]);
                    }
                }
                else if (choice == 'e')
                {
                    enText[i] = binToDecimal(cipher, 8, 128);
                }
                decrypt(cipher, decrypted, key1, key2);

                if (choice == 'd')
                {
                    decText[i] = binToDecimal(decrypted, 8, 128);
                }

                /*avoids future errors*/
                memset(binary, 0, strlen((char*)binary));
                memset(cipher, 0, strlen((char*)cipher));
                memset(decrypted, 0, strlen((char*)decrypted));
            }
            if (choice == 'p')
            {
                printf("%s", line);
            }
            else if (choice == 'd')
            { 
                printf("%s", decText);
                memset(decText, 0, strlen(decText));
            }
            else if (choice == 'e')
            { 
                printf("%s", enText);
                memset(enText, 0, strlen(enText));
            }
        }
    }   
    fclose(input);
}
void encrypt(int* binary, int* cipher, int* key1, int* key2)
{
    int i;
    int binaryR[4]; /*For E/P*/
    int binaryL[4];
    int sBoxPost1[4];
    int sBoxPost2[4];

    /*Initial Permutation (IP)*/
    initialPerm(binary, binaryL, binaryR);

    F(key1, binaryL, binaryR, sBoxPost1);

    /*WITH MY F FUNCTION, A SWAP FUNCTION IS NOT
    REQUIRED AS THEY ARE SWAPPED WITHIN*/

    F(key2, binaryR, sBoxPost1, sBoxPost2);


    /*Merge for inverse*/
    for (i = 0; i < 4; i++)
    {
        cipher[i] = sBoxPost2[i];
        cipher[i+4] = sBoxPost1[i];
    }

    inverseIP(cipher);  
}

void decrypt(int* cipher, int* decrypted, int* key1, int* key2)
{
    int i;
    int cipherL[4]; 
    int cipherR[4];
    int sBoxPost1[4];
    int sBoxPost2[4];

    /*Initial Permutation (IP)*/
    initialPerm(cipher, cipherL, cipherR);

    /*F Function (No Swap func req)*/
    F(key2, cipherL, cipherR, sBoxPost1);

    F(key1, cipherR, sBoxPost1, sBoxPost2);

    /*Merge for inverse*/
    for (i = 0; i < 4; i++)
    {
        decrypted[i] = sBoxPost2[i];
        decrypted[i+4] = sBoxPost1[i];
    }

    inverseIP(decrypted);
}

void decToBinary(int dec, int* binary, int bits)
{
    int i; /*ie. arr[7] to arr[0]*/
    for (i = bits - 1; i >= 0; i--)
    {
        binary[i] = dec % 2;
        dec = dec / 2;
    }
    /*decrements as needed to be in reverse*/
}

int binToDecimal(int* binary, int length, int num)
{
    int i, dec = 0;

    /*Goes through each bit, from num(128 if 8bit) to 1*/
    for (i = 0; i < length; i++)
    {
        if (binary[i] == 1)
        {
            dec += num;
        }
        /*If binary is 0 do nothing*/

        num = num / 2;
    }
    return dec;
}

void generateKey(int input, int* key1, int* key2)
{
    int i;
    int initKey[10] = {0,0,0,0,0,0,0,0,0,0}; /*init with 0's*/

    /*Key permutations*/
    int p10Pat[10] = {3,5,2,7,4,10,1,9,8,6}; /*patterns*/
    int p10[10];
    int p10Left[5];
    int p10Right[5];
    int p10Merge[10];
    int p10Merge2[10];

    int p8Pat[8] = {6,3,7,4,8,5,10,9};
    int p8[8];
    int p8_2[8];

    /*convert user input to binary*/
    decToBinary(input, initKey, 10);

    /*Permutate 10*/
    for (i = 0; i < 10; i++)
    {
        p10[i] = initKey[p10Pat[i]-1];
    }

    /*Split P10*/
    for (i = 0; i < 5; i++)
    {
        p10Left[i] = p10[i]; /*key1[0] to [4]*/
        p10Right[i] = p10[i+5]; /*key1[5] to [9]*/
    }

    /*Left Shift the split 5-bits*/
    leftShift(p10Left, 5);
    leftShift(p10Right, 5);

    /*Merge P10*/
    for (i = 0; i < 5; i++)
    {
        p10Merge[i] = p10Left[i];
        p10Merge[i+5] = p10Right[i];
    }

    /*Permutate 8*/
    for (i = 0; i < 8; i++)
    {
        p8[i] = p10Merge[p8Pat[i]-1];
        key1[i] = p8[i]; /*Key 1 is created*/
    }

    /*Double Left Shift P10 splits*/
    leftShift(p10Left, 5);
    leftShift(p10Left, 5);
    leftShift(p10Right, 5);
    leftShift(p10Right, 5);

    /*Merge P10 for key 2*/
    for (i = 0; i < 5; i++)
    {
        p10Merge2[i] = p10Left[i];
        p10Merge2[i+5] = p10Right[i];
    }

    /*Permutate 8*/
    for (i = 0; i < 8; i++)
    {
        p8_2[i] = p10Merge2[p8Pat[i]-1];
        key2[i] = p8_2[i]; /*Key 2 is created*/
    }       
    
}

void leftShift(int* binary, int length)
{
    int i, bit1;

    /*wrap around first element to last*/
    bit1 = binary[0];
    for (i = 0; i < length-1; i++)
    {
        binary[i] = binary[i+1];
    }
    
    binary[length-1] = bit1;    
}

void F(int* subKey, int* L, int* R, int* sBoxPost)
{
    int expandR[8]; /*for 4bit expand to 8bit*/

    /*Expansion/Permutation (E/P)*/
    expandPerm(R, expandR);

    /*XOR with Key*/
    XOR(expandR, subKey, 8);

    /*Use SBoxes and Permutate 4*/
    SBOX(expandR, sBoxPost);

    /*XOR sBoxPost with IP's L*/
    XOR(sBoxPost, L, 4);
}

void initialPerm(int* binary, int* binaryL, int* binaryR)
{
    int IP[8] = {2,6,3,1,4,8,5,7};
    int temp[8];
    int i;

    for (i = 0; i < 8; i++)
    {
        temp[i] = binary[i];
    }
    /*initial permutation*/
    for (i = 0; i < 8; i++)
    {
        binary[i] = temp[IP[i]-1];
    }

    /*Make left and right's for 4bit E/P*/
    for (i = 0; i < 4; i++)
    {
        binaryL[i] = binary[i];
        binaryR[i] = binary[i+4];
    }
}

void inverseIP(int* binary)
{
    int IP[8] = {4,1,3,5,7,2,8,6};
    int temp[8];
    int i;  

    for (i = 0; i < 8; i++)
    {
        temp[i] = binary[i];
    }
    for (i = 0; i < 8; i++)
    {
        binary[i] = temp[IP[i]-1];      
    }
}

void expandPerm(int* R, int* expandR)
{
    int EP[8] = {4,1,2,3,2,3,4,1}; /*Pattern*/
    int i;

    /*E/P*/
    for (i = 0; i < 8; i++)
    {
        expandR[i] = R[EP[i]-1];
    }
}

void XOR(int* binary, int* binary2, int bits)
{
    int i;
    int temp[8];
    for (i = 0; i < bits; i++)
    {
        /*     XOR:
            0 + 0 = 0 (0)
            0 + 1 = 1 (1)
            1 + 0 = 1 (1)
            1 + 1 = 0 (2)   */

        if (binary[i] + binary2[i] == 1)
        {
            temp[i] = 1;
        }
        else
        {
            temp[i] = 0;
        }
    }

    for (i = 0; i < bits; i++)
    {
        binary[i] = temp[i];
    }
}

void SBOX(int* expandR, int* sBoxPost)
{
    int S0[4][4] = {{1,0,3,2},{3,2,1,0},{0,2,1,3},{3,1,3,2}};
    int S1[4][4] = {{0,1,2,3},{2,0,1,3},{3,0,1,0},{2,1,0,3}};
    int P4_Pat[4] = {2,4,3,1};

    int bit1_4[2], bit2_3[2], bit5_8[2], bit6_7[2];
    int i, s0row, s0col, s1row, s1col;
    int s0result, s1result;
    int sBoxPost1[2], sBoxPost2[2];
    int temp[4];


    /*Left nibble, placed in 2 bit arrays for
    conversion to decimal, then sbox rows and
    columns are found */
    bit1_4[0] = expandR[0];
    bit1_4[1] = expandR[3];
    bit2_3[0] = expandR[1];
    bit2_3[1] = expandR[2];

    s0row = binToDecimal(bit1_4, 2, 2);
    s0col = binToDecimal(bit2_3, 2, 2);

    /*Right nibble*/
    bit5_8[0] = expandR[4];
    bit5_8[1] = expandR[7];
    bit6_7[0] = expandR[5];
    bit6_7[1] = expandR[6];
    s1row = binToDecimal(bit5_8, 2, 2);
    s1col = binToDecimal(bit6_7, 2, 2);

    s0result = S0[s0row][s0col];
    s1result = S1[s1row][s1col];

    /*Convert results to binary*/
    decToBinary(s0result, sBoxPost1, 2);
    decToBinary(s1result, sBoxPost2, 2);

    /*Join 2 bit arrays*/
    for (i = 0; i < 2; i++)
    {
        temp[i] = sBoxPost1[i];
        temp[i+2] = sBoxPost2[i];
    }

    /*Permutate 4*/
    for (i = 0; i < 4; i++)
    {
        sBoxPost[i] = temp[P4_Pat[i] - 1];
    }
}