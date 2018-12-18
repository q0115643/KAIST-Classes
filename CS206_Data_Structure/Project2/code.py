#######################################################################
# Data structure

class Node():
    def __init__(self,data):
        self.left = None
        self.right = None
        self.parent = None
        self.isAnimal = False
        self.data = data
        self.userInput = False

    def __str__(self):
        return self.data

class Stack():
    def __init__(self): # Create a stack
        self.stack = []
        self.size = 0

    def __str__(self):
        tmp = []
        for i in range(len(self.stack)):
            tmp.append(str(self.stack[i]))
        return str(tmp)

    def pop(self): # Pop element
        if self.isemptyStack():
            print("Empty Stack!")
            return None
        else:
            self.size -= 1
            return self.stack.pop()

    def push(self, input): # Push element
        self.stack.append(input)
        self.size += 1

    def top(self):
        if self.isemptyStack():
            print("Empty Stack!")
            return None
        return self.stack[-1]

    def isemptyStack(self): # Check whether the stack is empty
        return self.size == 0




################################################################################################
# Supporting functions and variables

AorB = ""
root = None
stackFrame = Stack()

def yesOrNo(string):
    while True:
        answer = input(string)
        if answer == 'Y' or answer == 'y':
            return True
        elif answer == 'N' or answer == 'n':
            return False
        else:
            print("Wrong input, please again")

def yesOrNoOrUndo(string):
    while True:
        answer = input(string)
        if answer == 'Y' or answer == 'y':
            return True
        elif answer == 'N' or answer == 'n':
            return False
        elif answer == 'U' or answer == 'u':
            return -1
        else:
            print("Wrong input, please again")


def setNode(left,right,item):
    item.left = left
    left.parent = item
    item.right = right
    right.parent = item


def init():
    f = open("data-A.txt",'w')
    f.write("Mouse\nTrout\nAre you a mammal?\n")
    f.close()
    f = open("data-B.txt",'w')
    f.write("Are you a mammal?\nMouse\nTrout\n")
    f.close()


################################################################################################

def finishgame():
    answer = yesOrNo("Shall we play again [ Y or N ] : ")
    if answer:
        play()
        return
    else:
        print("See you again!")
        return

def learn(current):
    name = input("I give up. What are you? ")
    question = input("Please type a 'Yes or No' question which can distinguish a " + name + " from a " + current.data + ".\nQuestion : ")
    if question[-1]!='?' :
        question = question + "?"
    answer = yesOrNo("As a " + name + ", " + question + " Please answer [ Y or N ] : ")

    oldAnimal = Node(current.data)
    oldAnimal.isAnimal = True
    newAnimal = Node(name)
    newAnimal.isAnimal = True

    current.data = question
    current.isAnimal = False
    newAnimal.parent = current
    oldAnimal.parent = current
    if answer:
        current.left = newAnimal
        current.right = oldAnimal
    else:
        current.left = oldAnimal
        current.right = newAnimal

    print("Thanks for teaching me!")
    writeFile()
    return current


def undo3(current):
    tmp = current
    while current.parent != None and not yesOrNo("Is the question '" + current.parent.data + "' is the question that you have made a mistake? [ Y or N ] : "):
        current = current.parent
    if current.parent == None:
        print("Then you have not made an wrong answer.")
        return tmp
    current = current.parent
    current.userInput = not current.userInput
    if current.userInput: return current.left
    else: return current.right

def undo(current):
    temp = current
    tmp = Stack()
    tmp.push(current)
    while current.parent != None:
        answer = yesOrNoOrUndo("Is the question '" + current.parent.data + "' is the question that you have made a mistake? [ Y or N or U ] : ")
        if answer == True:
            current = current.parent
            break
        elif answer == False:
            tmp.push(current)
            current = current.parent
        elif answer == -1:
            current = tmp.pop()
            if tmp.isemptyStack(): return current
        if current.parent == None:
            print("Then you have not mad an wrong answer.")
            return temp

    #current = current.parent
    current.userInput = not current.userInput
    if current.userInput : return  current.left
    else: return current.right


def undo2(current):
    tmp = Stack()
    tmp.push(stackFrame.pop())
    while stackFrame.size > 1 and tmp.size > 0:
        answer = yesOrNoOrUndo("Is the question '" + current.parent.data + "' is the question that you have made a mistake? [ Y or N ] : ")
        if answer == -1:
            current = tmp.pop()
            stackFrame.push(current)
        elif answer == True:
            current = current.parent



    pass


################################################################################################
# Related to file I/O

def readFile():
    global AorB
    while(True):
        AorB = input("Choose the data type [ A or B ] or [ X ] to reset the tree: ")
        if AorB == 'A' or AorB == 'B':
            break
        if AorB == 'X':
            init()
        else: print("Wrong input, please again.")

    if AorB == 'A': file = "data-A.txt"
    else : file = "data-B.txt"
    file = open(file,'r')
    stack = Stack()

    for l in file:
        line = l.strip()
        node = Node(line)
        node.isAnimal = line[-1]!='?'
        stack.push(node)

    if AorB == 'B':
        stack2 = Stack()
        while not stack.isemptyStack():
            stack2.push(stack.pop())
        stack = stack2

    file.close()
    #print(str(stack))
    return stack

def writeFile():
    current = root

    f = open("data-A.txt",'w')
    s1 = Stack()
    s2 = Stack()

    s1.push(current)

    while not s1.isemptyStack():
        current = s1.pop()
        s2.push(current)

        if current.left != None:
            s1.push(current.left)
        if current.right != None:
            s1.push(current.right)
    while not s2.isemptyStack():
        current = s2.pop()
        f.write(current.data+"\n")

    f.close()
    f = open("data-B.txt",'w')

    current = root
    s1.push(current)
    while not s1.isemptyStack():
        current = s1.pop()
        f.write(current.data+"\n")
        if current.right != None: s1.push(current.right)
        if current.left != None: s1.push(current.left)

    f.close()

######################################################################################


def buildTree(s):
    s1 = Stack()
    s2 = Stack()
    isPrevAnimal = False
    animalCnt = 0
    questionCnt = 0
    #print(str(s))
    if s.size == 1:
        node = Node(s.top().data)
        node.isAnimal = s.top().isAnimal
        node.root = s.pop()
        return node

    rootItem = s.pop()


    while animalCnt <= questionCnt:
        item = s.pop()
        if item.data[-1] == '?' : questionCnt+=1
        else : animalCnt+=1
        s1.push(item)
        if s.isemptyStack():
            s.push(s1.pop())
            break

    while not s1.isemptyStack():
        s2.push(s1.pop())

    #print(rootItem.data)
    sub1 = buildTree(s)
    sub2 = buildTree(s2)
    if AorB == 'A': setNode(sub1,sub2,rootItem)
    else: setNode(sub2,sub1,rootItem)
    return rootItem



def play():
    global root
    global stackFrame
    stackFrame = Stack()

    if root == None: root = buildTree(readFile())

    current = root
    stackFrame.push(current)

    answer = yesOrNo(current.data + " [ Y or N ] : ")
    if answer:
        current.userInput = True
        current = current.left
        stackFrame.push(current)
    else:
        current.userInput = False
        current = current.right
        stackFrame.push(current)

    while True:
        if not current.isAnimal:
            answer = yesOrNoOrUndo(current.data + " [ Y or N or U ] : ")
            if answer == True:
                current.userInput = True
                current = current.left
                stackFrame.push(current)
            elif answer == False:
                current.userInput = False
                current = current.right
                stackFrame.push(current)
            elif answer == -1:
                current = undo(current)

        else:
            answer = yesOrNoOrUndo("My guess is " + current.data + ". Am I right? [ Y or N or U ] : ")
            if answer == -1:
                current = undo(current)
            elif answer == True:
                finishgame()
                return
            elif answer == False:
                current = learn(current)
                finishgame()
                return



play()