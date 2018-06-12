% 1.exercise
% loading signals
[s, Fs] = audioread ('xstupi00.wav'); s = s';
% sampling frequency 
fprintf('Fs = %d [Hz]\n', Fs);
% count of samples
t = (0:(length(s)-1)) / Fs;
n = length(t);
fprintf('n = %d\n', n);
% duration time
t = n / Fs;
fprintf('t = %d (s)\n', t);
% -----------------------------------------------------
% 2.exercise
% https://www.mathworks.com/help/matlab/math/basic-spectral-analysis.html
n = length(s);
t = (0:n/2-1)/Fs;
y = fft(s); abs(y);
power = 10 * log10((abs(y)).^2/n);
f = (0:n/2-1)/n * Fs;
power = power(1:n/2);
plot (f, power); xlabel('f [Hz]'); ylabel ('PSD [dB]'); grid;
%clear f; clear s; clear y;
% -----------------------------------------------------
% 3.exercise
% https://www.mathworks.com/matlabcentral/answers/133957-finding-max-value-and-its-index
[max_value, index] = max(power);
index = index - 1;
fprintf('max_freq = %d [Hz]\n', index);
clear max_value; clear power;
% -----------------------------------------------------
% 4.exercise
% https://www.fit.vutbr.cz/study/courses/ISS/public/proj_studijni_etapa/3_sound/ukazmito.m
a = [1.000, 0.2289, 0.4662]; 
b = [0.2324, -0.4112, 0.2324];
figure; zplane(b, a); grid;
if ( isstable(b, a) )
    disp('STABLE')
else
    disp('UNSTABLE')
end
clear p; clear ans;
% -----------------------------------------------------
% 5.exercise
% https://www.fit.vutbr.cz/study/courses/ISS/public/proj_studijni_etapa/3_sound/ukazmito.m
H = freqz(b, a, Fs/2);
figure; plot (f, abs(H)); xlabel('f [Hz]'); ylabel('|H(f)|'); grid;
% -----------------------------------------------------
% 6.exercise
filter_signal = filter(b, a, s);
y = fft(filter_signal); abs(y);
power = 10 * log10((abs(y)).^2/n);
power = power(1:n/2);
plot (f, power); xlabel('f [Hz]'); ylabel ('PSD [dB]'); grid;
% -----------------------------------------------------
% 7.exercise
[max_value, index] = max(power);
index = index - 1;
fprintf('max_freq = %d [Hz]\n', index);
clear max_value; clear power;
% -----------------------------------------------------
% https://www.mathworks.com/help/matlab/ref/corrcoef.html
% 8.exercise
a = [1, 1, -1, -1];
A = repmat(a, 1, 80);
begin = 1;
finish = length(A);
max_coef = 0;
start_index = 1;
end_index = length(A);
    for i = 1:Fs-length(A),
        sig = s(begin:finish);
        r = corrcoef(abs(fft(sig)),abs(fft(A)));
        if ( r(1,2) > max_coef )
            max_coef = r(1,2); start_index = begin; end_index = finish;
	end
	begin = begin + 1;
	finish = finish + 1;
    end
clear r;
fprintf('============ %g\n', start_index);
start_index = start_index / Fs; end_index = end_index / Fs;
fprintf('max_coef = %g\n', max_coef);
fprintf('start_index = %g (s)\n', start_index);
fprintf('end_index = %g (s)\n', end_index);
% -----------------------------------------------------
% 9.exercise and 10.exercise
Rv = xcorr(s, 'biased'); k = -(n / length(A)):(n/length(A));
fprintf('R[10] = %g\n', Rv(n+10));
Rv = Rv(n-(n/length(A)):n+(n/length(A))); figure; plot(k, Rv); xlabel('k');
% -----------------------------------------------------
% 11.exercise
% https://www.fit.vutbr.cz/study/courses/ISS/public/proj_studijni_etapa/5_random/5_random.pdf
gmin = min(s); gmax = max(s);
counts = 50;
g = linspace(gmin, gmax, counts); g = g(:);
L = length(g);
N = length(s);
h = zeros(L, L);
bigy = repmat(s, L, 1);
bigx = repmat(g, 1, N);
[~, ind1] = min(abs(bigy - bigx));
ind2 = ind1(11:N);
for ii = 1:N-10,
    d1 = ind1(ii);
    d2 = ind2(ii); 
    h(d1, d2) = h(d1, d2) + 1;
end
surf = (g(2) - g(1))^2;
p = h / N / surf;
figure; bar3(p); axis([0 51 0 51 0 2]); xlabel('x1'); ylabel('x2');
% -----------------------------------------------------
% 13.exercise
check = sum(sum (p)) * surf; 
disp(['hist2: check -- 2d integral should be 1 and is ' num2str(check)]);
% -----------------------------------------------------
% 14.exercise
g = g(:); X1 = repmat(g, 1, L);
g = g'; X2 = repmat(g, L, 1);
r = sum(sum (X1 .* X2 .* p)) * surf;
fprintf('R[10] = %g\n', r);