 
function [ejex,ejey,ejez]=captura(numero_muestras)

close all;
clc;

%Vector donde se guardarán los datos
ejex=length(numero_muestras);
ejey=length(numero_muestras);
ejez=length(numero_muestras);

%Inicialización del puerto serie
%Cambiar el puerto serie por el usado
delete(instrfind({'Port'},{'COM3'}));
puerto_serial=serial('COM3');
puerto_serial.BaudRate=9600;
warning('off','MATLAB:serial:fscanf:unsuccessfulRead');

%Apertura del puerto serial
fopen(puerto_serial); 

%Contador del número de muestras ya tomadas
contador_muestras=1;

%Bucle while toma de muestras
while contador_muestras<=numero_muestras
        valor_ejex=fread(puerto_serial,1,'uint16');
        ejex(contador_muestras)=((valor_ejex(1))-512.0)/102.3;
        valor_ejey=fread(puerto_serial,1,'uint16');
        ejey(contador_muestras)=((valor_ejey(1))-512.0)/102.3;
        valor_ejez=fread(puerto_serial,1,'uint16');
        ejez(contador_muestras)=((valor_ejez(1))-512.0)/102.3;
        contador_muestras=contador_muestras+1;
end

%Cierre del puerto serial
fclose(puerto_serial); 
delete(puerto_serial);

end