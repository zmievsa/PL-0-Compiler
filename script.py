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

mine = """0               JMP     0       0       32
1               JMP     0       0       23
2               JMP     0       0       14
3               JMP     0       0       4
4               INC     0       0       5
5               LIT     0       0       1
6               STO     0       0       4
7               LOD     0       0       3
8               LOD     1       0       4
9               ADD     0       0       1
10              LOD     1       3       7
11              ADD     0       0       1
12              STO     0       2       3
13              RTN     0       0       0
14              INC     0       0       6
15              LOD     0       1       3
16              LOD     1       1       5
17              ADD     0       0       1
18              LOD     1       2       7
19              ADD     0       0       1
20              STO     0       0       3
21              CAL     0       0       4
22              RTN     0       0       0
23              INC     0       0       7
24              LIT     0       0       2
25              STO     0       1       5
26              LOD     0       1       5
27              LOD     1       1       7
28              ADD     0       0       1
29              STO     0       0       5
30              CAL     0       0       14
31              RTN     0       0       0
32              INC     0       0       8
33              LIT     0       0       1
34              STO     0       0       3
35              LIT     0       0       2
36              STO     0       0       4
37              LIT     0       0       3
38              STO     0       0       5
39              LIT     0       0       4
40              STO     0       0       6
41              LIT     0       0       5
42              STO     0       0       7
43              LOD     0       0       6
44              LOD     1       0       7
45              ADD     0       0       1
46              STO     0       0       3
47              LOD     0       0       5
48              SIO     0       0       1"""

# for line1, line2 in zip(strip(original).splitlines(), strip(mine).splitlines()):
#     if line1 != line2:
#         print(line1, ":::", line2)

for line in mine.splitlines():
    if "CAL" in line:
        print(line)