#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int val;
    char exp[100];
    bool legalNot;
} Element;

typedef struct {
    int kartu[4];
    bool isSolvable;
} combResult;

bool solved = false;
int totSolve = 0;
bool silentMode = false;

void solveBitwise24(Element arr[], int n);
void trySolve();
void checkPossibilities();

int main() {
    char instruction[20];
    printf("Apa yang mau kamu lakukan? (TRY_SOLVE / CHECK_POSSIBILITIES)\n");
    scanf("%s", instruction);

    if (strcmp(instruction,"TRY_SOLVE")==0) {
        trySolve();
    }
    else if (strcmp(instruction,"CHECK_POSSIBILITIES")==0) {
        checkPossibilities();
    }
    else {
        printf("INSTRUKSI TIDAK DIKENALI!\n");
    }
    return 0;
}

void solveBitwise24(Element arr[], int n) {
    if (silentMode && solved) return;

    int i,j,k;
    if (n == 1) {
        if (arr[0].val == 24) {
            solved = true;
            totSolve += 1;

            if (!silentMode) {
                printf(" Solusi %d: %s = 24 (11000)\n", totSolve, arr[0].exp);
            }
        }
        return;
    }

    for (i=0; i<n; i++) {
        if (arr[i].legalNot) {
            Element notArr[4];
            for (j=0; j<n; j++) {
                notArr[j] = arr[j];
            }

            notArr[i].val = (~arr[i].val) & 0x1F; //NOT
            char tempArr[100];
            sprintf(notArr[i].exp, "(~%s)", arr[i].exp);

            notArr[i].legalNot = false;
            solveBitwise24(notArr, n);

            if (silentMode && solved) return;
        }
    }

    for (i=0; i<n; i++) {
        for (j=0; j<n; j++) {
            if (i != j) {
                Element newArr[4];
                int idx = 0;

                for (k=0; k<n; k++) {
                    if (k!=i && k!=j) {
                        newArr[idx] = arr[k];
                        idx++;
                    }
                }
                newArr[idx].val = (arr[i].val & arr[j].val) & 0x1F; //AND
                sprintf(newArr[idx].exp, "(%s & %s)", arr[i].exp, arr[j].exp);
                newArr[idx].legalNot = true;
                solveBitwise24(newArr, n-1);
                if (silentMode && solved) return;

                newArr[idx].val = (arr[i].val | arr[j].val) & 0x1F;//OR
                sprintf(newArr[idx].exp, "(%s | %s)", arr[i].exp, arr[j].exp);
                newArr[idx].legalNot = true;
                solveBitwise24(newArr, n-1);
                if (silentMode && solved) return;
            }
        }
    }
}

void trySolve() {
    int i;
    int kartuInput[4];
    Element kartu[4];

    printf("Masukkan 4 nilai kartu (As=1, J=11, Q=12, K=13:)\n");
    for (i=0; i<4; i++) {
        printf("Kartu [%d]: ", i+1);
        scanf("%d", &kartuInput[i]);
        kartu[i].val = kartuInput[i] & 0x1F;
        sprintf(kartu[i].exp, "%d", kartuInput[i]);
        kartu[i].legalNot = true;
    }

    printf("\nMencari semua kemungkinan operasi...\n");
    solved = false;
    totSolve = 0;
    solveBitwise24(kartu, 4);

    if (!solved) {
        printf("\nHasil: Set kartu %d, %d, %d, %d UNSOLVABLE!\n", kartuInput[0],kartuInput[1],kartuInput[2],kartuInput[3]);
    }
    else {
        printf("\nHasil: Sukses menemukan %d variasi solusi (SOLVABLE)!\n", totSolve);
    }
}

void checkPossibilities() {
    combResult result[1825];
    int countSolve = 0;
    int countUnsolve = 0;
    int totalComb = 0;
    int k1,k2,k3,k4,i,j,k;

    silentMode = true;

    printf("MEMULAI ENUMERASI 1.820 KEMUNGKINAN SET KARTU...\n");
    for (k1=1; k1<=13; k1++) {
        for (k2=k1; k2<=13; k2++) {
            for (k3=k2; k3<=13; k3++) {
                for (k4=k3; k4<=13; k4++) {
                    result[totalComb].kartu[0] = k1;
                    result[totalComb].kartu[1] = k2;
                    result[totalComb].kartu[2] = k3;
                    result[totalComb].kartu[3] = k4;

                    Element Kartu[4];
                    int curVal[4] = {k1,k2,k3,k4};
                    for (i=0; i<4; i++) {
                        Kartu[i].val = curVal[i] & 0x1F;
                        sprintf(Kartu[i].exp, "%d", curVal[i]);
                        Kartu[i].legalNot = true;
                    }

                    solved = false;
                    totSolve = 0;
                    solveBitwise24(Kartu,4);

                    result[totalComb].isSolvable = solved;

                    if (solved) {
                        countSolve += 1;
                    }
                    else {
                        countUnsolve += 1;
                    }
                    totalComb += 1;
                }
            }
        }
    }
    FILE *fp = fopen("solvabilitas24Bitwise.csv", "w");
    if (fp == NULL) {
        printf("Error: Gagal membuat file!\n");
        return;
    }
    fprintf(fp, "=== TABEL SOLVED ===\n");
    fprintf(fp, "Kartu 1, Kartu 2, Kartu 3, Kartu 4, Jumlah Solusi\n");
    for (i=0; i<totalComb; i++) {
        if (result[i].isSolvable) {
            fprintf(fp, "%d,%d,%d,%d\n", result[i].kartu[0],result[i].kartu[1],result[i].kartu[2],result[i].kartu[3]);
        }
    }

    fprintf(fp, "\n\n");
    fprintf(fp, "=== TABEL SOLVED ===\n");
    fprintf(fp, "Kartu 1, Kartu 2, Kartu 3, Kartu 4\n");
    for (i=0; i<totalComb; i++) {
        if (!result[i].isSolvable) {
            fprintf(fp, "%d,%d,%d,%d\n", result[i].kartu[0],result[i].kartu[1],result[i].kartu[2],result[i].kartu[3]);
        }
    }
    fclose(fp);

    float solvePercent, unsolvePercent;
    solvePercent = ((float)countSolve/totalComb)*100.0;
    unsolvePercent = 100.0 - solvePercent;

    printf("\nSUKSES! Data 1.820 kombinasi telah diekspor ke 'solvabilitas24Bitwise.csv'!\n");
    printf("RINGKASAN DATA STATISTIK\n");
    printf("Total Kombinasi Set Kartu Unik : %d\n", totalComb);
    printf("Persentase Set Kartu SOLVABLE: %d Set (%.2f%%)\n", countSolve, solvePercent);
    printf("Persentase Set Kartu UNSOLVABLE: %d Set (%.2f%%)\n", countUnsolve, unsolvePercent);
    silentMode = false;
}
