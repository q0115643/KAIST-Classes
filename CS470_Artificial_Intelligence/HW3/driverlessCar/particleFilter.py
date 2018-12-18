# 20140174

'''
Licensing Information: Please do not distribute or publish solutions to this
project. You are free to use and extend Driverless Car for educational
purposes. The Driverless Car project was developed at Stanford, primarily by
Chris Piech (piech@cs.stanford.edu). It was inspired by the Pacman projects.
'''
from engine.const import Const
import util
import random
import math

# Class: Particle Filter
# ----------------------
# Maintain and update a belief distribution over the probability of a car
# being in a tile using a set of particles.
class ParticleFilter(object):
    
    NUM_PARTICLES = 50
    
    # Function: Init
    # --------------
    # Constructer that initializes an ExactInference object which has
    # numRows x numCols number of tiles.
    def __init__(self, numRows, numCols):
        ''' initialize any variables you will need later '''
        self.belief = util.Belief(numRows, numCols, 0.0)
        self.particleDict = {} # position to number of particles in that position
        for i in range(0, self.NUM_PARTICLES):
            r = random.randint(0, numRows-1)
            c = random.randint(0, numCols-1)
            if (r, c) in self.particleDict:
                self.particleDict[(r, c)] += 1.0
            else:
                self.particleDict[(r, c)] = 1.0

    # Function: Observe
    # -----------------
    # Updates beliefs based on the distance observation and your agents position.
    # The noisyDistance is a gaussian distribution with mean of the true distance
    # and std = Const.SONAR_NOISE_STD.The variable agentX is the x location of 
    # your car (not the one you are tracking) and agentY is your y location.
    def observe(self, agentX, agentY, observedDist):
        ''' your code here'''
        weightDict = {}
        weightedParticleSum = 0.0

        for pos, num in self.particleDict.iteritems():
            row = pos[0]
            col = pos[1]
            distX = util.colToX(col) - agentX
            distY = util.rowToY(row) - agentY
            dist = math.sqrt(distX * distX + distY * distY)  # true distance
            EbarX = util.pdf(dist, Const.SONAR_STD, observedDist) # weight

            p = EbarX * num
            weightDict[pos] = p
            weightedParticleSum += p
            self.particleDict[pos] = p

        # Resample
        particleNum = 0
        for pos, p in self.particleDict.iteritems():
            num = math.floor(p * self.NUM_PARTICLES / weightedParticleSum)
            particleNum += int(num)
            self.belief.setProb(pos[0], pos[1], num / self.NUM_PARTICLES)
            self.particleDict[pos] = num

        for i in range(0, self.NUM_PARTICLES - particleNum):
            pos = util.weightedRandomChoice(weightDict)
            self.particleDict[pos] += 1.0
            self.belief.addProb(pos[0], pos[1], 1.0 / self.NUM_PARTICLES)

    # Function: Elapse Time
    # ---------------------
    # Update your inference to handle the passing of one heartbeat. Use the
    # transition probability you created in Learner  
    def elapseTime(self):
        ''' your code here'''
        transProb = util.loadTransProb()
        newDict = {}
        for pos, num in self.particleDict.iteritems():
            row = pos[0]
            col = pos[1]
            self.belief.setProb(row, col, 0.0)
            if pos in transProb:
                for newPos, transP in transProb[pos].iteritems():
                    if newPos in newDict:
                        newDict[newPos] += transP * num
                    else:
                        newDict[newPos] = transP * num
        self.particleDict = newDict

    # Function: Get Belief
    # ---------------------
    # Returns your belief of the probability that the car is in each tile. Your
    # belief probabilities should sum to 1.    
    def getBelief(self):
        return self.belief
