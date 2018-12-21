#include <stdio.h>
#include <stdlib.h>

void compareFiles(char *fp1_name_chunk, char *fp2_name_chunk, char *fp3_name_chunk)
{
	char fp1_name[256];
	snprintf(fp1_name, sizeof fp1_name, "%s%s", "./test-cases/self-test-results/", fp1_name_chunk);
	char fp2_name[256];
	snprintf(fp2_name, sizeof fp2_name, "%s%s", "./test-cases/self-test-results/", fp2_name_chunk);
	char fp3_name[256];
	snprintf(fp3_name, sizeof fp3_name, "%s%s", "./test-cases/self-test-results/", fp3_name_chunk);
	FILE *fp1 = fopen(fp1_name, "r");
	FILE *fp2 = fopen(fp2_name, "r");
	FILE *fp3 = fopen(fp3_name, "r");
	if (fp1 == NULL || fp2 == NULL || fp3 == NULL)
	{
		printf("Error : Files not open");
		exit(0);
	}
	char ch1 = getc(fp1);
	char ch2 = getc(fp2);
	char ch3 = getc(fp3);
	while (ch1 != EOF && ch2 != EOF && ch3 != EOF)
	{
		if (ch1 != ch2 || ch2 != ch3 || ch1 != ch3)
		{
			printf("ERROR: <%s> and <%s> and <%s> are not identical\n", fp1_name_chunk, fp2_name_chunk, fp3_name_chunk);
			exit(-1);
		}
		ch1 = getc(fp1);
		ch2 = getc(fp2);
		ch3 = getc(fp3);
	}

	printf("CORRECT: <%s> and <%s> and <%s> are identical\n", fp1_name_chunk, fp2_name_chunk, fp3_name_chunk);
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
}

int main(int argc, char *argv[])
{
	compareFiles(argv[1], argv[2], argv[3]);
	return 0;
}