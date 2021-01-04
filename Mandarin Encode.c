#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
int _setmode (
    int fd,
    int mode
);

struct encode
{
    int byte1;//save the char
    int by[3];
    int encode[20];
    int letterlength;//total number of the char's encode
    int codelength;
} e[256]= {0};

int main(int argc ,char *argv[])
{
    setmode( fileno( stdin ), _O_BINARY );
    int i=0,j,k,m,r,temp1,getbyte,b1,total=0,index=0,num,queue[40],ch;

    FILE *fin,*fcodebook,*fout;

    fin=fopen(argv[1],"rb");
    fcodebook=fopen(argv[2],"rb");
    fout=fopen(argv[3],"w+b");
    while((getbyte=fgetc(fcodebook))!=EOF)
    {
        if(getbyte==92) //get '\'
        {
            e[i].by[0]=getbyte;
            getbyte=fgetc(fcodebook);
            if(getbyte=='n'||getbyte=='r') //if get '\r','\n'
            {
                e[i].by[1]=getbyte;
                e[i].letterlength=2;
                getbyte=fgetc(fcodebook); //get ' '
                for(m=0;; m++)
                {
                    getbyte=fgetc(fcodebook);
                    if(getbyte=='\r') //if get '\r' means need to store the next symbol
                    {
                        break;
                    }
                    e[i].encode[m]=getbyte;
                    e[i].codelength++;
                }
            }
            i++;
        }
        else if(getbyte!='\n'&&getbyte!='\r')
        {
            e[i].by[0]=getbyte;
            temp1=getbyte;
            getbyte=fgetc(fcodebook);
            if(getbyte==' ')
            {
                e[i].letterlength=1;

                if(temp1==' '&&getbyte==' ') //store the encode of space
                {
                    for(m=0;; m++)
                    {
                        getbyte=fgetc(fcodebook);
                        if(getbyte=='\r')
                        {
                            break;
                        }
                        e[i].encode[m]=getbyte;
                        e[i].codelength++;
                    }
                    i++;
                }
                else
                {
                    for(m=0;; m++)
                    {
                        getbyte=fgetc(fcodebook);
                        if(getbyte=='\r')  //if get '\r' means need to store the next symbol
                        {
                            break;
                        }
                        e[i].encode[m]=getbyte;
                        e[i].codelength++;
                    }
                    i++;
                }
                continue;
            }
            else
            {
                e[i].by[1]=getbyte;      //get mandarin's byte
                getbyte=fgetc(fcodebook);
                e[i].by[2]=getbyte;
                e[i].letterlength=3;
                getbyte=fgetc(fcodebook); //get ' '
                for(m=0;; m++)
                {
                    getbyte=fgetc(fcodebook);
                    if(getbyte=='\r')
                    {
                        break;
                    }
                    e[i].encode[m]=getbyte;
                    e[i].codelength++;
                }
                i++;
                continue;
            }
        }
    }
    num=i;
//let e.by[] become e.byte1
    for(i=0; i<num; i++)
    {

        if(e[i].letterlength==1)
        {
            e[i].byte1=e[i].by[0];
        }
        else if(e[i].letterlength==2)
        {
            if(e[i].by[1]==110)
                e[i].byte1='\n';
            else if(e[i].by[1]==114)
                e[i].byte1='\r';
        }
        else if(e[i].letterlength==3)
        {
            e[i].byte1=(e[i].by[0]-48)*100+(e[i].by[1]-48)*10+(e[i].by[2]-48)*1;
        }
    }
    k=0;
    r=0;
    while((getbyte=fgetc(fin))!=EOF)
    {
        for(i=0; i<num; i++)
        {
            if(getbyte==e[i].byte1)
            {

                k+=e[i].codelength;

                for(m=0; m<e[i].codelength; m++)
                {
                    total++;//count the total number of char
                    queue[r]=e[i].encode[m];
                    r++;
                }
                while (k>=8)
                {
                    ch=0;
                    for(j=0; j<8; j++) //let queue[0]->queue[7] become 8bits
                    {

                        ch+=(queue[j]-48)*pow(2,(7-j));
                    }
                    fprintf(fout,"%c",ch);
                    if(k>=8)
                    {
                        for(m=0; m<16; m++) //choosing 16 is because the waiting queue won't greater than 16
                            queue[m]=queue[m+8];
                    }
                    k-=8;
                    r-=8;
                }
            }
        }
    }
    for(i=0; i<k; i++) //display the last elements in queue
        fprintf(fout,"%c",queue[i]);

    fclose(fin);
    fclose(fcodebook);
    fclose(fout);

    return 0;
}
