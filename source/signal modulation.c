






#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>




/*
модуляция одного wav-ника другим. Допустим, есть два звуковых потока:
несущий и модулирующий. В модулирующем мы определяем периоды колебаний
(банально по переходу между с минуса на плюс) и в зависимости от этого
периода замедляем или ускоряем несущий звуковой поток. Для
замедления/ускорения используем билинейную интерполяцию. В
модулирующем сигнале принимаем, что минимальный период — 1/100 с.,
максимальный — 1/10000 с. Частоту несущего сигнала замедляем тоже не
до 0, а до какого-то определенного значения. Например, в 100 раз.
*/

int main(){

int16_t *signal=malloc(100000000);
int16_t *envelop=malloc(100000000);
int16_t *sral=malloc(100000000);

FILE *in=fopen("signal.raw","rb");
FILE *in2=fopen("mod.raw","rb");
FILE *out=fopen("out.raw","wb");

int size_sig=fread(signal,2,50000000,in);
int size_env=fread(envelop,2,50000000,in2);

int q;
double pos=0;
for(q=0;q<size_env;q++){
pos+=(double)envelop[q]/10000.0;
if(pos<0){pos=0;}
if(pos>=size_sig){pos=size_sig;}
sral[q]=signal[(int)pos];
}

fwrite(sral,2,size_env,out);

fclose(out);
fclose(in);
fclose(in2);

return EXIT_SUCCESS;
}








