def strip(s):
    return s.replace("\t", "").replace(" ", "").lower()

original = """0		jmp	0	0	25
1		jmp	0	0	2
2		inc	0	0	4
3		lod	0	1	4
4		sto	0	0	3
5		lod	0	1	4
6		lit	1	0	1
7		sub	0	0	1
8		sto	0	1	4
9		lod	0	1	4
10		lit	1	0	0
11		eql	0	0	1
12		jpc	0	0	15
13		lit	0	0	1
14		sto	0	1	3
15		lod	0	1	4
16		lit	1	0	0
17		gtr	0	0	1
18		jpc	0	0	20
19		cal	0	1	2
20		lod	0	1	3
21		lod	1	0	3
22		mul	0	0	1
23		sto	0	1	3
24		rtn	0	0	0
25		inc	0	0	5
26		lit	0	0	3
27		sto	0	0	4
28		cal	0	0	2
29		lod	0	0	3
30		sio	0	0	1
31		sys	0	0	3"""

mine = """0               JMP     0       0       25
1               JMP     0       0       2
2               INC     0       0       4
3               LOD     0       1       4
4               STO     0       0       3
5               LOD     0       1       4
6               LIT     1       0       1
7               SUB     0       0       1
8               STO     0       1       4
9               LOD     0       1       4
10              LIT     1       0       0
11              EQL     0       0       1
12              JPC     0       0       15
13              LIT     0       0       1
14              STO     0       1       3
15              LOD     0       1       4
16              LIT     1       0       0
17              GTR     0       0       1
18              JPC     0       0       20
19              CAL     0       1       2
20              LOD     0       1       3
21              LOD     1       0       3
22              MUL     0       0       1
23              STO     0       1       3
24              RTN     0       0       0
25              INC     0       0       5
26              LIT     0       0       3
27              STO     0       0       4
28              CAL     0       0       2
29              LOD     0       0       3
30              SIO     0       0       1
31              SIO     0       0       3"""

for line1, line2 in zip(strip(original).splitlines(), strip(mine).splitlines()):
    if line1 != line2:
        print(line1, ":::", line2)