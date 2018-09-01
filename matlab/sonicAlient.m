dst = [0,429,710,1710];% distance, unit cm
ras = [7300,8956,11700,16839];%rasing edge time, unit us
fal = [8100,12300,16700,27460];%falling edge time,unit us

timeused = fal - ras;

%plot(dst,ras,'r',dst,fal,'g',dst,timeused,'y');
x = 0:1000:12000;
p1 = 0.1745;%0.1745 0.01669, 0.01822
p2 =  -140.1;%-150.1 -196.9, -103.3
y = p1*x + p2;
plot(timeused,dst,'-*r',x,y);