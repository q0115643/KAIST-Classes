
class Matrix():
    def __init__(self):
        self.array = []


    def __str__(self):
        answer = ""
        for i in range(len(self.array)):
            for j in range(len(self.array[0])):
                answer += str(self.array[i][j]) + " "
            answer += "\n"
        return answer

    def add(self, other):
        if self.row != other.row or self.col != other.col:
            print("Matrix size is different!")
            return None
        else:
            answerMatrix = Matrix()
            answer = []
            for i in range(self.row):
                tmp = []
                for j in range(self.col):
                    tmp.append(self.array[i][j]+other.array[i][j])
                answer.append(tmp)
            answerMatrix.array = answer
            answerMatrix.row = len(answer)
            answerMatrix.col = len(answer[0])
            return answerMatrix

    def mult(self,other):
        if self.col != other.row:
            print("Matrix size is inappropriate")
            return None
        else:
            answerMatrix = Matrix()
            answer = []
            for i in range(self.row):
                tmp = []
                for j in range(other.col):
                    tmp.append(0)
                    for k in range(self.col):
                        tmp[-1] += self.array[i][k] * other.array[k][j]
                answer.append(tmp)
            answerMatrix.array = answer
            answerMatrix.row = len(answer)
            answerMatrix.col = len(answer[0])
            return answerMatrix

    def transpose(self):
        answerMatrix = Matrix()
        answer = []
        for i in range(self.col):
            tmp = []
            for j in range(self.row):
                tmp.append(0)
            answer.append(tmp)

        for i in range(self.row):
            for j in range(self.col):
                answer[j][i] = self.array[i][j]
        answerMatrix.array = answer
        answerMatrix.row = len(answer)
        answerMatrix.col = len(answer[0])
        return answerMatrix

    def isSymmetric(self):
        if self.row != self.col:
            print("The matrix is not square matrix, it cannot be symmetric")
            return False
        else:
            for i in range(self.row):
                for j in range(i,self.col):
                    if self.array[i][j] != self.array[j][i]: return False
            return True

    def isSame(self, other):
        if self.row != other.row or self.col != other.col:
            print("The dimension of the matrix is different")
            return False
        else:
            for i in range(self.row):
                for j in range(self.col):
                    if self.array[i][j] != other.array[i][j]: return False
            return True

    def power(self,n):
        if n == 1:
            return self
        else: return self.mult(self.power(n-1))


class NewMatrix(Matrix):
    def __init__(self, filename):
        self.array = []
        file = open(filename, 'r')
        self.row = 0
        for l in file:
            tmp = l.strip().split()
            self.col = len(tmp)
            for i in range(self.col):
                tmp[i] = int(tmp[i])
            self.array.append(tmp)
            self.row += 1

class IdentityMatrix(Matrix):
    def __init__(self, n):
        self.array=[]
        for i in range(n):
            tmp = []
            for j in range(n):
                if i == j:
                    tmp.append(1)
                else:
                    tmp.append(0)
            self.array.append(tmp)

############################################
# Part III
###  Problem 1
#####  (1)
fa = "a.txt"
fb = "b.txt"

a = NewMatrix(fa)
b = NewMatrix(fb)
print("A = \n" + str(a))
print("B = \n" + str(b))
print("A + B = \n" + str(a.add(b)))

##### (2)
fa = "c.txt"
fb = "d.txt"

a = NewMatrix(fa)
b = NewMatrix(fb)
print("A = \n" + str(a))
print("B = \n" + str(b))
print("A*B = \n" + str(a.mult(b)))

###  Problem 2
fa = "e.txt"
fb = "f.txt"

a = NewMatrix(fa)
b = NewMatrix(fb)
print("A = \n" + str(a))
print("B = \n" + str(b))
print("A*B = \n" + str(a.mult(b)))

print("I = \n" + str(IdentityMatrix(3)))
print(IdentityMatrix(3).array == a.mult(b).array)
print()

###  Problem 3
fa = "e.txt"

a = NewMatrix(fa)
print("A = \n" + str(a))
print("A^10 = \n" + str(a.power(10)))