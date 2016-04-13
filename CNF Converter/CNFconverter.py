#
# Import package argv from system for 
# using parameteres
from sys import argv
#
script,mode,file_name = argv
#
#
#************************************************
# Function to convert implies
#************************************************
# 
# Start convert implies
#
import time

def convertimplies(mylist):
    if len(mylist) == 3:
        mynewlist=['or',['not',mylist[1]],mylist[2]]
    return mynewlist
#
# End convert implies
#
#************************************************
# Function to convert double implies
#************************************************
#
# Start convert double implies
#
def convertiff(mylist):
    if len(mylist) == 3:
        mynewlist = ['and',['or',mylist[1],['not',mylist[2]]],['or',mylist[2],['not',mylist[1]]]]
    return mynewlist
#
# End double implies
#
#************************************************
# Recursive function to perform Step1 of conversion 
# of implies and double implies for entire string
#************************************************
#
#Start step1
#
def step1(mylist,count):
    for i in range(0,count):
        if isinstance(mylist[i],list):
            mylist[i] = step1(mylist[i],len(mylist[i]))
        elif mylist[i] == 'implies' and i == 0:
            mylist = convertimplies(mylist)

            mylist = step1(mylist,len(mylist))
        elif mylist[i] == 'iff' and i == 0:
            mylist = convertiff(mylist)
            mylist = step1(mylist,len(mylist))
        cnfsentence = mylist
    return cnfsentence
#
#End step1
#
#
#************************************************
# Function for Not propagation of the list 
# sent either recursively or through step2
#************************************************
#
#
def notpropagation(mylist):
    myresult = []
    for i in range(0,len(mylist)):
        #print 'i inside not prop',i
        #print 'inside for loop in notprop ',mylist[i]
        if mylist[i] == 'not':
            if len(mylist[i+1]) > 1:
                mynewlist = step2(mylist[i+1],len(mylist[i+1]),0)
                return mynewlist
            else:
                mynewlist = mylist[i+1]
                myresult.append(mynewlist)
                return mynewlist
        if isinstance(mylist[i],list):
            mynewlist = notpropagation(mylist[i])
            myresult.append(mynewlist)
        elif mylist[i] == 'and':
            myresult.append('or')
        elif mylist[i] == 'or':
            myresult.append('and') 
        else:
            mynewlist = ['not',mylist[i]]
            myresult.append(mynewlist)
        if i >= 1 and len(myresult) == 1:
            myresult = myresult[0]
        #print'My result inside not',myresult
    if len(myresult) == 1:
        myresult = myresult[0]  
    return myresult
#
#
#************************************************
# Main function to parse through the list 
# to check for eligibility of not propagation
#************************************************
#
#
#
def step2(mylist,count,negindex):
    myresult = []
    for i in range(0,count):
        #print 'i inside step2',i
        #print 'negindex', negindex
        #print 'Mylist inside for',mylist[i]
        if  mylist[i] =='not' and negindex == 0:
            negindex = 1
        elif isinstance(mylist[i],list) and negindex == 1:
            mynewlist = notpropagation(mylist[i])
            myresult.append(mynewlist)
        elif isinstance(mylist[i],list):
            mynewlist = step2(mylist[i],len(mylist[i]),negindex)
            myresult.append(mynewlist)
        elif negindex == 1:
            mynewlist = ['not',mylist[i]]
            myresult.append(mynewlist)
        else:
            mynewlist = mylist[i]
            myresult.append(mynewlist)
        #print 'My result inside step 2 before if',myresult
        if i >= 1 and len(myresult) == 1:
            myresult = myresult[0]
        #print'My result inside step2',myresult
    if len(myresult) == 1:
        myresult = myresult[0]
    return myresult
#
# End Step 2
#
#************************************************
# function that moves ops recursively inwards
# and distributes them in form op ,a, b
#************************************************
#
#
def mvopin(operator,mylist,lastele):
    myresult = []
    if len(mylist) >= 3:
        myresult.insert(0,operator)
        myresult.insert(1,mvopin(operator,mylist[0:len(mylist)-1],mylist[len(mylist)-1]))
        myresult.insert(2,mylist[len(mylist)-1])
    else:
        myresult = mylist
        myresult.insert(0,operator)
    return myresult
#
#************************************************
# Intermediary function that calls the move ops 
# inwards function and returns result to step A
#************************************************
#
def simplify(mylist,count):
    myresult = []
    mynewlist = []
    myresult.insert(0,mylist[0])
    myresult.insert(1,mvopin(mylist[0],mylist[1:count-1],mylist[count-1]))
    myresult.insert(2,mylist[count-1])
    return myresult
#
#************************************************
# Step 3A to parse through the entire list
# modify existing lists with values greater
# 
#************************************************
#
# Start Step 3A
#
def step3a(mylist,count):
    myresult = []
    if len(mylist) > 3:
            mynewlist = simplify(mylist,len(mylist))
            return mynewlist
    for i in range(0,count):
        if isinstance(mylist[i],list):
            mynewlist = step3a(mylist[i],len(mylist[i]))
        else:
            mynewlist = mylist[i]
        myresult.append(mynewlist)
        #print 'My result in step 3A',myresult
    return myresult
#
# End Step 3A
#
#************************************************
# Is distribute function suggest if the operators
# are different that is and or for each member of 
# the list
#************************************************
#
#
def isdistribute(mylist):
    indicator = 0
    if len(mylist) > 2:
        if mylist[0] == 'or':
            for i in range(1,len(mylist)):
                if len(mylist[i]) > 1:
                    if mylist[i][0] == 'and':
                        indicator = 1
                        return indicator
    return indicator
#
#
#************************************************
# distribute ors over ands by applying the 
# distribution law
#************************************************
#
#
def distributelaw(mylist):
    #print "\nLIST:\t %s" % mylist
    #time.sleep(0.2)
    mynewlist = []
    #print 'My List in distribute law',mylist
    if len(mylist) > 2:
        if mylist[0] == 'or':
            if len(mylist[1])>2 and len(mylist[2])>2:
                if mylist[1][0] == 'and' and mylist[2][0] == 'and':
                    mynewlist.append('and')
                    mynewlist.append(step3b(['or',mylist[1][1],mylist[2][1]]))
                    mynewlist.append(step3b(['or',mylist[1][1],mylist[2][2]]))
                    mynewlist.append(step3b(['or',mylist[1][2],mylist[2][1]]))
                    mynewlist.append(step3b(['or',mylist[1][2],mylist[2][2]]))
                elif mylist[1][0] == 'and' and mylist[2][0] == 'or':
                    if isdistribute(mylist[2]) == 1:
                       mylist[2] = step3b(mylist[2])
                    mynewlist.append('and')
                    mynewlist.append(step3b(step3a(['or',mylist[1][1],mylist[2][1],mylist[2][2]],4)))
                    mynewlist.append(step3b(step3a(['or',mylist[1][2],mylist[2][1],mylist[2][2]],4)))
                elif mylist[2][0] == 'and' and mylist[1][0] == 'or':
                    if  isdistribute(mylist[1]) == 1:
                        mylist[1] = step3b(mylist[1])
                    mynewlist.append('and')
                    mynewlist.append(step3b(step3a(['or',mylist[2][1],mylist[1][1],mylist[1][2]],4)))
                    mynewlist.append(step3b(step3a(['or',mylist[2][2],mylist[1][1],mylist[1][2]],4)))
            elif len(mylist[1]) > 2 and len(mylist[2])<3:
                if mylist[1][0] == 'and':
                    mynewlist.append('and')
                    mynewlist.append(step3b(['or',mylist[1][1],mylist[2]]))
                    mynewlist.append(step3b(['or',mylist[1][2],mylist[2]]))
                elif mylist[1][0] == 'or':
                    if  isdistribute(mylist[1]) == 1:
                        mylist[1] = step3b(mylist[1])
                    mynewlist.append(mylist[1])
            elif len(mylist[2]) > 2 and len(mylist[1])<3:
                if mylist[2][0] == 'and':
                    mynewlist.append('and')
                    mynewlist.append(step3b(['or',mylist[2][1],mylist[1]]))
                    mynewlist.append(step3b(['or',mylist[2][2],mylist[1]]))
                elif mylist[2][0] == 'or':
                    if isdistribute(mylist[1]) == 1:
                        mylist[1] = step3b(mylist[1])
                    mynewlist.append(mylist[1])
#        if isdistribute(mynewlist[1]) == 1:
#            mynewlist[1] = step3b(mynewlist[1])
#        if isdistribute(mynewlist[2]) == 1:
#            mynewlist[2] = step3b(mynewlist[2])
    #print 'My new list in distribute law',mynewlist
    #print "\nNEW LIST:\t %s" % mynewlist
    return step3a(mynewlist,len(mynewlist))
#************************************************
# Wrapper function to indicate to move and outwards
# and ors inwards
#************************************************
#
# Start Step 3B
#
def step3b(mylist):
    distributeind = 0
    if len(mylist) > 2:
        distributeind = isdistribute(mylist)
    #
    #if distributeind == 1:
        #print 'mylist in step 3b before distribute law',mylist
     #   mylist = distributelaw(mylist)
    for i in range(1,len(mylist)):
        if len(mylist[i]) > 2:
            mylist[i] = step3b(mylist[i])
    #
    distributeind = isdistribute(mylist)
    if distributeind == 1:
        mylist = distributelaw(mylist)
    return step3a(mylist,len(mylist))
#
# End Step 3B
#
#
#
#************************************************
# Function to clean up the additional operators 
# and remove duplicates
#************************************************
#
#
def cleanup(mylist):
    result = []
    if len(mylist) > 1:
        if mylist[0] == 'not':
            return mylist
        else:
            op = mylist[0]
            result.append(op)
            for i in range(1,len(mylist)):
                if mylist[i][0] == op:
                    for j in range(1,len(mylist[i])):
                        if mylist[i][j] not in result:
                            result.append(mylist[i][j])
                else:
                    result.append(mylist[i])
    else:
        result.append(mylist)
    return result

#
#************************************************
# Wrapper function to clean up additional operators
#************************************************
#
#
def step4(mylist,count):
    mylist = cleanup(mylist)
    for i in range(1, len(mylist)):
        if len(mylist[i]) > 1:
            mylist[i] = step4(mylist[i],len(mylist[i]))
    mylist = cleanup(mylist)
    return mylist
#
#
#************************************************
# Remove Duplicates
#************************************************
#
#
def rem_dup(mylist):
    clause = mylist[:]
    for i in range(1,len(mylist)):
        count = 0
        count = clause.count(mylist[i])
        if count > 1:
            while count != 1:
                clause.remove(mylist[i])
                count = count-1
    return clause
#
#************************************************
# Wrapper function to remove duplicates
#************************************************
#
#
def step5(mylist):
    # Remove duplicates from the inner lists
    mynewlist =[]
    mynewlist.append(mylist[0])
    #print mylist
    for i in range(1,len(mylist)):
        mylist[i] = rem_dup(mylist[i])
        mynewlist.append(mylist[i])
    return mynewlist
#
#
#************************************************
# find duplicate clauses 
# This has been referenced from stack overflow to 
# check each element in each list
#************************************************
#
def find_dup_clauses(clause2,clause1):
    check_clause = clause1[:]
    for key in clause2:
        try:
            check_clause.remove(key)
        except ValueError:
            return False
    return not check_clause
#
#************************************************
# Wrapper Function for CNF conversion steps
#************************************************
#
# Start cnf convert
#
def cnfconvert(rawsentence,count):
    #
    # step 1
    # Remove implies and double implies in the first step of cnf conversion
    #
    global cnfsentence
    cnfsentence = rawsentence
    if len(rawsentence) == 1:
        return rawsentence[0]
    cnfsentence = step1(rawsentence,count)
    #print'Step 1 : ',cnfsentence
    #
    # step 2
    # Move negation inwards
    #
    count = len(cnfsentence)
    cnfsentence = step2(cnfsentence,count,0)
    #print'Step 2 : ',cnfsentence 
    #

    # step 3
    #
    # Divided into two parts
    # Part A
    #
    # modify the sentence to move the operators inwards such that
    # each element would be stored in a format op ,a , b
    # if a and b are nested lists perform each operation such that
    # consider  elements a,b,c
    # store the format as op,(op , a ,b ),c
    #
    count = len(cnfsentence)
    cnfsentence = step3a(cnfsentence,count)
    #print 'Step 3a : ',cnfsentence
    #
    # Part B
    #
    # Apply the distribution laws such that a V (b & c)
    # is converted to a V B & a V c
    #
    cnfsentence = step3b(cnfsentence)
    #print 'Step 3b : ',cnfsentence
    #
    # Step 4
    # 
    # Clean up to remove additional operators
    # [or[or a b] c] = [or a b c]
    #
    count = len(cnfsentence)
    cnfsentence = step4(cnfsentence,count)
    #print 'Step 4 : ',cnfsentence
    #
    # Step 5
    # Check for duplicates and remove them from the CNF sentences
    #
    if len(cnfsentence) > 2:
        cnfsentence = step5(cnfsentence)
    if len(cnfsentence) == 2 and cnfsentence[0] != 'not':
        cnfsentence = cnfsentence[1]
    # After duplicates are remove in each clause
    # Check for and/or operators in each clause
    # and check if the length of that element is 2
    newcnfsentence = []
    if len(cnfsentence) > 2:
        newcnfsentence.append(cnfsentence[0])
        for key in cnfsentence[1:]:
            if len(key) == 2:
                if key[0] == 'not':
                    newcnfsentence.append(key)
                else:
                    newcnfsentence.append(key[1])
            else:
                newcnfsentence.append(key)
    elif len(cnfsentence) == 2 and cnfsentence[0] != 'not':
        newcnfsentence = cnfsentence[1]
    else:
        newcnfsentence = cnfsentence[:]
    # Remove duplicate clauses in the cnfsentence
    #
    dup_clause_free = []
    duplicates_sentence = newcnfsentence[:]
    #print 'new Cnf sentence',newcnfsentence
    #print 'Duplicate sentences',duplicates_sentence
    if len(newcnfsentence) > 2:
        dup_clause_free.append(newcnfsentence[0])
        dup_clause_free.append(newcnfsentence[1])
        for clause1 in duplicates_sentence[1:]:
            if len(clause1) > 1:
                dupl_ind = 0
                for clause2 in dup_clause_free[1:]:
                    if find_dup_clauses(clause2,clause1):
                        dupl_ind = 1
                if dupl_ind == 0:
                    dup_clause_free.append(clause1)
            else:
                if clause1 not in dup_clause_free:
                    dup_clause_free.append(clause1)
    elif len(newcnfsentence) == 2 and newcnfsentence[0] != 'not':
        dup_clause_free = newcnfsentence[1]
    else:
        dup_clause_free = newcnfsentence[:]

    if len(dup_clause_free) == 2 and dup_clause_free[0] != 'not':
        dup_clause_free = dup_clause_free[1]

    return dup_clause_free
#
# End cnf convert
#
# Open file
#
file_inst = open(file_name,'r')
line_cnt = file_inst.readline()
line_cnt = int(line_cnt)
file_name = 'sentences_CNF.txt'
output_file = open(file_name,'w')
#Check count of line 
for i in range(0,line_cnt) :
    line = file_inst.readline()
    line = line.strip()
    line = eval(line)
    #line = ['iff', ['implies', ['and', 'A', ['implies', 'B', ['not', 'A']]], ['and', ['not', 'C'], 'D']], ['iff', ['iff', 'X', 'Y'], ['iff', ['implies', 'P', 'Q'], ['iff', ['and', 'A', ['not', 'G']], 'Y']]]]
    count = len(line)
    convsent = cnfconvert(line,count)
    cnfsentence = str(convsent).replace('\'','"')
    output_file.write(cnfsentence)
    output_file.write('\n')
#print 'Line ',line
#print 'i',i

