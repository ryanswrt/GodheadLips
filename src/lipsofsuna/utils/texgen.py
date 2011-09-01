import Image
from math import *

def beckmannimg():
	def beckmannval(n, m):
		def x(n, m):
			t = tan(acos(n))
			d = m*pow(n,4.0)
			return 1.0/d*exp(-(t*t)/m)
		s = (1.0, 0.12, 0.023, 0.012)
		return [x(n, m * m * v) for v in s]
	def beckmanntex(x, y):
		b = beckmannval(x, y)
		c = [int(255 * 0.5 * pow(v, 0.1)) for v in b]
		return (c[0], c[1], c[2], c[3])
	w = 512
	h = 512
	img = Image.new("RGBA", (w, h))
	pix = img.load()
	for x in range(0, w):
		for y in range(0, h):
			b = beckmanntex(float(x + 1) / float(w), float(y + 1) / float(h))
			pix[(x, y)] = b
	img.save("beckmann1.png")

beckmannimg()
