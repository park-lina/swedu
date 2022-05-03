import sys

def Input_Data():
	readl = sys.stdin.readline
	W, H = map(int, readl().split())
	N = int(readl())
	info = [list(map(int, readl().split())) for _ in range(N+1)]
	return N, W, H, info

sol = -1
# 입력받는 부분
N, W, H, info = Input_Data()
 
# 여기서부터 작성
def Solve(N, W, H, info):
    my_loc, _dist1 = info[-1][0], info[-1][1]

    if my_loc == 1 or my_loc == 2:
        _dist2 = W - _dist
    else: 
        _dist2 = H - _dist
    
    dists = 0
    for i in range(N-1):
        loc, dist1 = info[i][0], info[i][1]
        
        if loc == 1 or loc == 2:
            dist2 = W - dist1
        else:
            dist2 = H - dist1
            
        
# 출력하는 부분
print(sol)
