#include <stdio.h>
int main(void)
{
	 FILE *fp;
   char ch;
   int i;
   fp = fopen("file.txt","w+");
   fputs("This is tutorialspoint.com", fp);
  
   fseek( fp, 2, SEEK_SET );
   for(i=33;i<45;i++)
   {
	ch = fputc(i,fp);
	printf("------------ch:: %c \n",ch);
   
   }
   ch = fgetc(fp);
   //printf("------------ch:: %c \n",ch);
   //fputs(" C Programming Langauge", fp);
   fclose(fp);
   
   return(0);
}
