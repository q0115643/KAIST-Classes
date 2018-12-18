#######################################################################
# Data structure

class dNode():
    def __init__(self,weight, vertex2):
        self.prev = None
        self.next = None
        self.to = vertex2
        self.weight = weight
        self.degree = -1


class WUG():
    def __init__(self):
        # O(1)

        # edge의 hash table 생성
        # key := (vertex1, vertex2) // 양방향 모두 가능
        # data := dNode(weight = given weight, to = vertex2, prev = None, next = None) or None(when there is no edge for given vertices)
        # 두 vertice를 key로 받아서 weight와 향하는 vertex를 정보로 갖고 있는 Node를 찾을 수 있는 hash table
        self.edge = dict()

        # vertex의 hash table 생성
        # key := (Object(vertex))
        # data := dNode(weight = given weight, to = vertex2)
        # vertex의 adjacency list를 vertex들 사이의 순서 없이 갖고 있음; 각각의 vertex를 key로 받음
        self.vertices = dict()

        # vertex의 list 생성
        # data := 입력으로 들어온 vertices
        self.verticesList = []

        self.vertexcount = 0
        self.edgecount = 0

    def vertexCount(self): # O(1)
        return self.vertexcount # return the number of vertices in the graph

    def edgeCount(self): # O(1)
        return self.edgecount # return the number of edges in the graph

    def getVertices(self): # O(1)
        return self.verticesList # return all vertices

    def addVertex(self, vertex): 
        self.vertices[vertex] = dNode(vertex, None)
        self.vertices[vertex].degree = 0
        self.verticesList.append(vertex)
        self.vertexcount+=1

    def removeVertex(self, vertex):
        current = self.vertices[vertex].next
        while current != None:
            tmp = current
            current = current.next
            self.removeEdge(vertex, tmp.to)

        self.verticesList.remove(vertex)
        del self.vertices[vertex]
        self.vertexcount-=1


    def isVertex(self, vertex):
        return vertex in self.vertices

    def degree(self, vertex):
        return self.vertices[vertex].degree

    def getNeighbors(self, vertex):
        result = []
        current = self.vertices[vertex].next
        while current != None:
            result.append(current.to)
            current = current.next

        return result

    def addEdge(self, vertex1, vertex2, weight):
        #for adding edge (vertex1,vertex2)
        self.edge[(vertex1, vertex2)] = dNode(weight, vertex2)

        self.edge[(vertex1, vertex2)].next = self.vertices[vertex1].next
        if self.vertices[vertex1].next != None:
            self.vertices[vertex1].next.prev = self.edge[(vertex1,vertex2)]
        self.vertices[vertex1].next = self.edge[(vertex1,vertex2)]
        self.edge[(vertex1, vertex2)].prev = self.vertices[vertex1]

        # for adding edge (vertex2,vertex1)
        self.edge[(vertex2, vertex1)] = dNode(weight, vertex1)

        self.edge[(vertex2, vertex1)].next = self.vertices[vertex2].next
        if self.vertices[vertex2].next != None:
            self.vertices[vertex2].next.prev = self.edge[(vertex2, vertex1)]
        self.vertices[vertex2].next = self.edge[(vertex2, vertex1)]
        self.edge[(vertex2, vertex1)].prev = self.vertices[vertex2]
        # adjust edgecount and degrees
        self.edgecount+=1
        self.vertices[vertex1].degree+=1
        self.vertices[vertex2].degree+=1

    def removeEdge(self, vertex1, vertex2):
        # doubly-linked adjacency list 갱신
        self.edge[(vertex1,vertex2)].prev.next = self.edge[(vertex1,vertex2)].next
        if self.edge[(vertex1,vertex2)].next != None:
            self.edge[(vertex1,vertex2)].next.prev = self.edge[(vertex1,vertex2)].prev

        self.edge[(vertex2, vertex1)].prev.next = self.edge[(vertex2, vertex1)].next
        if self.edge[(vertex2, vertex1)].next != None:
            self.edge[(vertex2, vertex1)].next.prev = self.edge[(vertex2, vertex1)].prev

        # dictionary에서 제거
        del self.edge[(vertex1,vertex2)]
        del self.edge[(vertex2,vertex1)]

        self.edgecount-=1
        self.vertices[vertex1].degree-=1
        self.vertices[vertex2].degree-=1


    def isEdge(self, vertex1, vertex2):
        return (vertex1, vertex2) in self.edge

    def weight(self, vertex1, vertex2):
        if self.isEdge(vertex1, vertex2):
            return self.edge[(vertex1,vertex2)].weight
        else:
            print("There is no edge that links given vertices")
            return None

def test():
    graph = WUG()
    graph.addVertex('a')
    graph.addVertex('b')
    graph.addVertex('c')
    graph.addVertex('d')
    graph.addVertex('e')
    graph.addVertex('z')
    graph.addVertex('A')
    print(graph.isVertex('A'))
    graph.removeVertex('A')

    print(graph.vertexCount())
    print(graph.edgeCount())
    print(graph.getVertices())

    print(graph.isVertex('A'))
    print(graph.degree('z'))

    graph.addEdge('a','b',4)
    graph.addEdge('a','c',2)
    graph.addEdge('b','c',1)
    graph.addEdge('b','e',5)
    graph.addEdge('c','e',8)
    graph.addEdge('c','d',10)
    graph.addEdge('e','d',2)
    graph.addEdge('e','z',6)
    graph.addEdge('d','z',3)
    graph.addEdge('a','z',100)

    print(graph.weight('d','z'))
    print(graph.isEdge('e','z'))
    print(graph.isEdge('a','z'))
    print(graph.degree('a'))
    print(graph.edgeCount())
    print(graph.getNeighbors('z'))
    graph.removeEdge('z','a')
    print(graph.isEdge('z','a'))
    print(graph.getNeighbors('a'))
    print(graph.degree('a'))


def test2():
    graph = WUG()
    graph.addVertex('a')
    graph.addVertex('b')
    graph.addVertex('c')
    graph.addVertex('d')
    graph.addVertex('e')
    graph.addVertex('f')

    graph.addEdge('a','f',5)
    graph.addEdge('a','b',3)
    graph.addEdge('a','d',4)
    graph.addEdge('b','d',3)
    graph.addEdge('b','c',1)
    graph.addEdge('b','f',1)
    graph.addEdge('c','d',2)
    graph.addEdge('d','e',3)
    graph.addEdge('d','f',2)
    graph.addEdge('e','f',2)
    return graph


def test3():
    graph = WUG()
    graph.addVertex('a')
    graph.addVertex('b')
    graph.addVertex('c')
    graph.addVertex('d')

    graph.addEdge('a','b',4)
    graph.addEdge('c','d',1)
    graph.addEdge('b','c',10)

    return graph

def makeEdgeList(g):
    list = []
    vertices = g.getVertices()

    for i in range(len(vertices)): # O(|E|)
        tmp = g.getNeighbors(vertices[i])
        for j in range(len(tmp)):
            if ((tmp[j], vertices[i]), g.weight(vertices[i],tmp[j])) not in list:
                list.append(((vertices[i],tmp[j]), g.weight(vertices[i],tmp[j])))



    list = sorted(list, key=lambda pair: pair[1])

    return list

def minSpanTree(g):
    tree = WUG()


    edgeList = makeEdgeList(g)
    vertices = g.getVertices()
    forest = []

    for vertex in vertices:
        forest.append(set(vertex))

    #print(edgeList)


    while tree.edgeCount() < g.vertexCount()-1 and len(edgeList) > 0:
        edge = edgeList.pop(0)
        (isNotCycle, forest) = cyclecheck(forest,edge)
        if isNotCycle:
            if not tree.isVertex(edge[0][0]):
                tree.addVertex(edge[0][0])
            if not tree.isVertex(edge[0][1]):
                tree.addVertex(edge[0][1])
            tree.addEdge(edge[0][0], edge[0][1], edge[1])

    return tree

def cyclecheck(forest,edge): # O(|e|) since there is at most |e| length list 'forest'
    first = None
    second = None
    for i in range(len(forest)):
        if edge[0][0] in forest[i] and edge[0][1] in forest[i]:
            return (False,forest)
        elif edge[0][0] in forest[i]: first = i
        elif edge[0][1] in forest[i]: second = i
    forest[first] = forest[first] | forest[second]
    forest.pop(second)
    return (True,forest)

print(makeEdgeList(minSpanTree(test3())))