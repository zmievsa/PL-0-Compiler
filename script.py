def strip(s):
    return s.replace("\t", "").replace(" ", "").lower()

original = """
0	jmp	0	0	32
1	jmp	0	0	23
2	jmp	0	0	14
3	jmp	0	0	4
4	inc	0	0	5
5	lit	0	0	1
6	sto	0	0	4
7	lod	0	0	3
8	lod	1	0	4
9	add	0	0	1
10	lod	1	3	7
11	add	0	0	1
12	sto	0	2	3
13	rtn	0	0	0
14	inc	0	0	6
15	lod	0	1	3
16	lod	1	1	5
17	add	0	0	1
18	lod	1	2	7
19	add	0	0	1
20	sto	0	0	3
21	cal	0	0	4
22	rtn	0	0	0
23	inc	0	0	7
24	lit	0	0	2
25	sto	0	1	5
26	lod	0	1	5
27	lod	1	1	7
28	add	0	0	1
29	sto	0	0	5
30	cal	0	0	14
31	rtn	0	0	0
32	inc	0	0	8
33	lit	0	0	1
34	sto	0	0	3
35	lit	0	0	2
36	sto	0	0	4
37	lit	0	0	3
38	sto	0	0	5
39	lit	0	0	4
40	sto	0	0	6
41	lit	0	0	5
42	sto	0	0	7
43	lod	0	0	6
44	lod	1	0	7
45	add	0	0	1
46	sto	0	0	3
47	lod	0	0	5
48	sio	0	0	1
49	cal	0	0	23
50	sys	0	0	3"""

mine = """
0		JMP	0	0	32
1		JMP	0	0	23
2		JMP	0	0	14
3		JMP	0	0	4
4		INC	0	0	5
5		LIT	0	0	1
6		STO	0	0	4
7		LOD	0	0	3
8		LOD	1	0	4
9		ADD	0	0	1
10		LOD	1	3	7
11		ADD	0	0	1
12		STO	0	2	3
13		RTN	0	0	0
14		INC	0	0	6
15		LOD	0	1	3
16		LOD	1	1	5
17		ADD	0	0	1
18		LOD	1	2	7
19		ADD	0	0	1
20		STO	0	0	3
21		CAL	0	0	4
22		RTN	0	0	0
23		INC	0	0	7
24		LIT	0	0	2
25		STO	0	1	5
26		LOD	0	1	5
27		LOD	1	1	7
28		ADD	0	0	1
29		STO	0	0	5
30		CAL	0	0	14
31		RTN	0	0	0
32		INC	0	0	8
33		LIT	0	0	1
34		STO	0	0	3
35		LIT	0	0	2
36		STO	0	0	4
37		LIT	0	0	3
38		STO	0	0	5
39		LIT	0	0	4
40		STO	0	0	6
41		LIT	0	0	5
42		STO	0	0	7
43		LOD	0	0	6
44		LOD	1	0	7
45		ADD	0	0	1
46		STO	0	0	3
47		LOD	0	0	5
48		SIO	0	0	1
49		CAL	0	0	23
"""

for line1, line2 in zip(strip(original).splitlines(), strip(mine).splitlines()):
    if line1 != line2:
        print(line1, ":::", line2)
    else:
        print(line1, ":::", line2)

# for line in mine.splitlines():
#     if "CAL" in line:
#         print(line)