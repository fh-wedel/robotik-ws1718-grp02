Kpr = 0.39088;
Kir = 0.98867;
Kdr = 0.01398;

s = tf('s');
n = 4.75;
d = [0.5 1];
Gs = tf(n,d);
PID = Kpr + Kir/s + Kdr * s;


figure(1);
hold on;
for g = [1 10 15 20 25 30] 
    Go = Gs * PID * g;
    nyquist(Go);
end;
hold off;
grid on;