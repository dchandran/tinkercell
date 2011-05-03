function score = detectOscillation(y)
	x = autocor(y); %get correlations
	t0 = min(find (x < 0));  %find first place where x crosses 0 correlation
	x2 = x(t0:end); %get everything after that point
	l = length(x2);
	score = (0.75 * sum(x2 > 0.4) + 0.25 * sum(x2 < -0.5))/l; %just a made-up scoring function

