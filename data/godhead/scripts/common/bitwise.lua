Bitwise = Class()

Bitwise.bchk = function(clss, x, y)
	return x % (y + y) >= y
end

Bitwise.band = function(clss, x, y)
	local v = 0
	for b = 0,31 do
		if Bitwise:bchk(x,2^b) and Bitwise:bchk(y,2^b) then v = v + 2^b end
	end
	return v
end

Bitwise.bor = function(clss, x, y)
	local v = 0
	for b = 0,31 do
		if Bitwise:bchk(x,2^b) or Bitwise:bchk(y,2^b) then v = v + 2^b end
	end
	return v
end
