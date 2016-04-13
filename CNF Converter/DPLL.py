#
# Import package argv from system for 
# using parameteres
from sys import argv
from collections import defaultdict
#
script,mode,file_name = argv
#
#
#************************************
# make symbols function converts the
# existing sets of clauses to a set of
# symbols 
#************************************
#
def make_symbols(myclauses):
    symbol_list = []
    for i in range(0,len(myclauses)):
        clause_symbols = []
        symbol = []
        clause_symbols = myclauses[i]
        for i in range(0,len(clause_symbols)):
            symbol = clause_symbols[i]
            if symbol not in symbol_list:
                symbol_list.append(symbol)
    return symbol_list
#
#
#************************************
# make clauses function converts the 
# existing sentence to a list of clauses
# SETS of SET
#************************************
def make_clauses(sentence):
    clause_list = []
    if sentence[0] == 'or':
        clause_list.append(sentence[1:])
        return clause_list
    elif sentence[0] == 'and':
        for i in range(1,len(sentence)):
            clause = []
            if sentence[i][0] == 'or' :
                clause = sentence[i]
                clause_list.append(clause[1:])
            else:
                clause.append(sentence[i])
                clause_list.append(clause)
        return clause_list
    else:
        clause_list.append(sentence)
        return clause_list

#************************************
# make model function stores the boolean
# for each symbol in ordered dictionary
#************************************ 
#
#
#
def make_model(mysymbols):
    model_list = defaultdict()
    for i in range(0,len(mysymbols)):
        if len(mysymbols[i]) > 1:
            symbol = mysymbols[i][1]
        else:
            symbol = mysymbols[i]
        model_list[symbol] = 0
    return model_list
#
#
#
#
#*************************************
# Check clauses for false values
#*************************************
#
#
def check_clause_false(clause,model):
    for i in range(0,len(clause)):
#        print 'Check Clause False',clause[i]
        if len(clause[i]) > 1:
            if model[clause[i][1]] == 'false':
#                print 'Check clause false return false'
                return False
        else:
            if model[clause[i]] == 'true':
#                print 'Check clause false return false'
                return False
    return True
#
#
#*************************************
# Check model for true values
#*************************************
#
#
def check_model_true(clauses,model):
    for i in range(0,len(clauses)):
        if check_clause_false(clauses[i],model):
#            print 'Check model true return false'
            return False
#    print 'Check model true return true'
    return True
#
#
#
#*************************************
# Check clauses for true values
#*************************************
#
#
def check_false(clause,model):
    for i in range(0,len(clause)):
#        print 'Check False for false model',clause[i]
        if len(clause[i]) > 1:
            if model[clause[i][1]] == 'false' or model[clause[i][1]] == 0:
#                print 'Check clause true return false'
                return False
        else:
            if model[clause[i]] == 'true' or model[clause[i]] == 0:
#                print 'Check clause true return false'
                return False
#    print 'Check false model return true'
    return True
#
#
#
#*************************************
# Check model for false values
#*************************************
#
def check_model_false(clauses,model):
    for i in range(0,len(clauses)):
        if check_false(clauses[i],model):
#            print 'Check model false return True'
            return True
#    print 'Check model false return true'
    return False
#
#
#
#*************************************
# Check if the symbol is pure
#*************************************
#
def check_pure_symbol(symbols):
    for i in range(0,len(symbols)):
        #print '',symbols[i]
        if len(symbols[i]) > 1:
            if symbols[i][1] not in symbols:
                return symbols[i]
        else:
            value = ['not',symbols[i]]
            if value not in symbols:
                return symbols[i]
    return 0
#
#
#*************************************
# Check if the clause has a unit literal
#*************************************
#
#
def check_unit_clause(clauses):
    for i in range(0,len(clauses)):
        if len(clauses[i]) == 1:
            return clauses[i]
    return 0

#
#*************************************
# Recursive DPLL algorithm to check 
# satisfiability
#*************************************
#
#
def dpll(clauses,symbols,model):
    if check_model_true(clauses,model):
       return True
    if check_model_false(clauses,model):
       return False
    #
    # Check if pure symbol then recursively call dpll    
    #
    #print 'Symbols ', symbols
    pure_symbol = check_pure_symbol(symbols)
    if pure_symbol != 0:
        if len(pure_symbol) > 1:
            literal = pure_symbol[1]
            value  = 'false'
        else:
            literal = pure_symbol
            value = 'true'
        model[literal] = value
        symbols.remove(pure_symbol)
        #print'After remove ',symbols
        return dpll(clauses,symbols,model)
    
    # Check if a clause has only one unit which is a literal
    #  then recursively call dpll
    #
    unit_literal = check_unit_clause(clauses)
    if unit_literal != 0:
        if len(unit_literal) == 1:
            unit_literal = unit_literal[0]
    #
    #
    if unit_literal != 0:
        if len(unit_literal) > 1:
            literal = unit_literal[1]
            value  = 'false'
        else:
            literal = unit_literal
            value = 'true'
        model[literal] = value
        symbols.remove(unit_literal)
        if ['not',literal] in symbols:
            symbols.remove(['not',literal])
        #print'After remove ',symbols
        return dpll(clauses,symbols,model)
    #
    # Apply the DPLL algorithm recursively
    # by picking
    #
    new_symbols = symbols[:]
    if len(symbols[0]) > 1:
        new_symbols.remove(symbols[0])
        literal = symbols[0][1]
        if literal in new_symbols:
            new_symbols.remove(literal)
        value  = 'false'
    else:
        literal = symbols[0]
        new_symbols.remove(literal)
        if ['not',literal] in new_symbols:
            new_symbols.remove(['not',literal])
        value = 'true'
    model[literal] = value

    first_call = dpll(clauses,new_symbols,model)
    if value == 'true':
        value = 'false'
    else:
        value = 'true'
    model[literal] = value 
    second_call = dpll(clauses,new_symbols,model)
    return first_call or second_call
    
#***********************************
# Write the output in the file
#***********************************
def write_file(final_output):
    output_file.write(final_output)
    output_file.write('\n')
#
#
#***********************************
# Create format of final output
#***********************************
#
#
def create_final_list(model,truth_value):
    output = []
    if truth_value:
        output.append('true')
        for key in model:
            if model[key] == 0:
                model[key] = 'false'
            value = key+'='+model[key]
            output.append(value)
    else:
        output.append('false')
    return output


#
#******************************
# Wrapper function to perform DPLL
# operations and return the desired 
# result to file
#******************************
#
def dpllwrapper(sentence):
    main_model = defaultdict()
    main_symbols = []
    main_clauses = []
    final_output = []
    truth_val    = 1
    
    # Check for single element
    #print sentence
    if len(sentence) == 1:
        element = sentence[0]
        element = element+'=true'
        final_output = ['true',element]
    # Check for single negation element
    elif len(sentence) == 0:
        final_output = ['true']
    elif len(sentence) == 2 and sentence[0] == 'not':
        element = sentence[1]
        element = element+'=false'
        final_output = ['true',element]
    else:
        #
        # make clauses of the cnf sentence
        #
        main_clauses = make_clauses(sentence)
        #
        # Creation of symbols for each sentence
        
        main_symbols = make_symbols(main_clauses) 
    
        # Creation of model for each of the symbols 
        # using default dictionary
        main_model = make_model(main_symbols)
    
        if sentence[0] == 'or':
            lit_ind = 0
            # parse through the main_symbols to find single literals and assign true
            # 
            for i in range(0,len(main_symbols)):
                if len(main_symbols[i]) == 1:
                    lit_ind = 1
                    main_model[main_symbols[i]] = 'true'
                    break
            # if only negations present in the main symbol assign one to true others to false
            # other wise assign false to everyone
            if lit_ind == 0:
                main_model[main_symbols[0][1]] = 'false'
            for i in main_model:
                if main_model[i] == 0:
                    main_model[i] = 'true'
    
            truth_val = True
        else:
            truth_val = dpll(main_clauses,main_symbols,main_model)
         
        final_output = create_final_list(main_model,truth_val)
    #
    # Write into the file
    #
    str_output = str(final_output)
    str_output = str_output.replace('\'','"')
    write_file(str_output)

#
#
#
#******************************
# Open file
#******************************
#
file_inst = open(file_name,'r')
line_cnt = file_inst.readline()
line_cnt = int(line_cnt)
file_name = 'CNF_satisfiability.txt'
output_file = open(file_name,'w')
#Check count of line 
for i in range(0,line_cnt):
    line = file_inst.readline()
    line = line.strip()
    sentence = eval(line)
    #sentence = ['or',['not','B'],['not', 'A']]
    #sentence = ['and', ['or', 'A', 'B', 'C', 'D', ['not', 'E'], 'F'], ['or', 'A', 'B', 'C', ['not', 'A'], ['not', 'E'], 'F'], ['or', 'A', 'B', 'C', ['not', 'B'], ['not', 'E'], 'F'], ['or', 'A', 'B', 'C', ['not', 'C'], ['not', 'E'], 'F'], ['or', 'A', 'B', 'C', 'D', ['not', 'F'], 'E'], ['or', 'A', 'B', 'C', ['not', 'A'], ['not', 'F'], 'E'], ['or', 'A', 'B', 'C', ['not', 'B'], ['not', 'F'], 'E'], ['or', 'A', 'B', 'C', ['not', 'C'], ['not', 'F'], 'E'], ['or', ['not', 'D'], 'D', ['not', 'E'], 'F'], ['or', ['not', 'D'], 'D', ['not', 'F'], 'E'], ['or', 'A', 'B', 'C', 'D', ['not', 'G'], 'H', ['not', 'I'], 'J'], ['or', 'A', 'B', 'C', 'D', ['not', 'G'], 'H', ['not', 'J'], 'I'], ['or', 'A', 'B', 'C', 'D', ['not', 'H'], 'G', ['not', 'I'], 'J'], ['or', 'A', 'B', 'C', ['not', 'A'], ['not', 'G'], 'H', ['not', 'I'], 'J'], ['or', 'A', 'B', 'C', ['not', 'B'], ['not', 'G'], 'H', ['not', 'I'], 'J'], ['or', 'A', 'B', 'C', ['not', 'A'], ['not', 'G'], 'H', ['not', 'J'], 'I'], ['or', 'A', 'B', 'C', ['not', 'B'], ['not', 'G'], 'H', ['not', 'J'], 'I'], ['or', 'A', 'B', 'C', ['not', 'C'], ['not', 'G'], 'H', ['not', 'I'], 'J'], ['or', 'A', 'B', 'C', ['not', 'C'], ['not', 'G'], 'H', ['not', 'J'], 'I'], ['or', 'A', 'B', 'C', ['not', 'A'], ['not', 'H'], 'G', ['not', 'I'], 'J'], ['or', 'A', 'B', 'C', ['not', 'B'], ['not', 'H'], 'G', ['not', 'I'], 'J'], ['or', 'A', 'B', 'C', ['not', 'C'], ['not', 'H'], 'G', ['not', 'I'], 'J'], ['or', 'A', 'B', 'C', 'D', ['not', 'H'], 'G', ['not', 'J'], 'I'], ['or', 'A', 'B', 'C', ['not', 'A'], ['not', 'H'], 'G', ['not', 'J'], 'I'], ['or', 'A', 'B', 'C', ['not', 'B'], ['not', 'H'], 'G', ['not', 'J'], 'I'], ['or', 'A', 'B', 'C', ['not', 'C'], ['not', 'H'], 'G', ['not', 'J'], 'I'], ['or', ['not', 'D'], 'D', ['not', 'G'], 'H', ['not', 'I'], 'J'], ['or', ['not', 'D'], 'D', ['not', 'G'], 'H', ['not', 'J'], 'I'], ['or', ['not', 'D'], 'D', ['not', 'H'], 'G', ['not', 'I'], 'J'], ['or', ['not', 'D'], 'D', ['not', 'H'], 'G', ['not', 'J'], 'I'], ['or', ['not', 'D'], ['not', 'A'], ['not', 'E'], 'F'], ['or', ['not', 'D'], ['not', 'B'], ['not', 'E'], 'F'], ['or', ['not', 'D'], ['not', 'A'], ['not', 'F'], 'E'], ['or', ['not', 'D'], ['not', 'B'], ['not', 'F'], 'E'], ['or', ['not', 'D'], ['not', 'C'], ['not', 'E'], 'F'], ['or', ['not', 'D'], ['not', 'C'], ['not', 'F'], 'E'], ['or', ['not', 'D'], ['not', 'A'], ['not', 'G'], 'H', ['not', 'I'], 'J'], ['or', ['not', 'D'], ['not', 'A'], ['not', 'G'], 'H', ['not', 'J'], 'I'], ['or', ['not', 'D'], ['not', 'A'], ['not', 'H'], 'G', ['not', 'I'], 'J'], ['or', ['not', 'D'], ['not', 'B'], ['not', 'G'], 'H', ['not', 'I'], 'J'], ['or', ['not', 'D'], ['not', 'B'], ['not', 'G'], 'H', ['not', 'J'], 'I'], ['or', ['not', 'D'], ['not', 'B'], ['not', 'H'], 'G', ['not', 'I'], 'J'], ['or', ['not', 'D'], ['not', 'A'], ['not', 'H'], 'G', ['not', 'J'], 'I'], ['or', ['not', 'D'], ['not', 'B'], ['not', 'H'], 'G', ['not', 'J'], 'I'], ['or', ['not', 'D'], ['not', 'C'], ['not', 'G'], 'H', ['not', 'I'], 'J'], ['or', ['not', 'D'], ['not', 'C'], ['not', 'G'], 'H', ['not', 'J'], 'I'], ['or', ['not', 'D'], ['not', 'C'], ['not', 'H'], 'G', ['not', 'I'], 'J'], ['or', ['not', 'D'], ['not', 'C'], ['not', 'H'], 'G', ['not', 'J'], 'I'], ['or', 'E', 'F', 'G', 'H', ['not', 'A'], 'D'], ['or', 'E', 'F', 'G', ['not', 'G'], ['not', 'A'], 'D'], ['or', 'E', 'F', ['not', 'H'], 'H', ['not', 'A'], 'D'], ['or', 'E', ['not', 'E'], 'G', 'H', ['not', 'A'], 'D'], ['or', 'E', ['not', 'E'], 'G', ['not', 'G'], ['not', 'A'], 'D'], ['or', 'E', ['not', 'E'], ['not', 'H'], 'H', ['not', 'A'], 'D'], ['or', 'E', 'F', ['not', 'H'], ['not', 'G'], ['not', 'A'], 'D'], ['or', 'E', ['not', 'E'], ['not', 'H'], ['not', 'G'], ['not', 'A'], 'D'], ['or', ['not', 'F'], 'F', 'G', 'H', ['not', 'A'], 'D'], ['or', ['not', 'F'], 'F', 'G', ['not', 'G'], ['not', 'A'], 'D'], ['or', ['not', 'F'], 'F', ['not', 'H'], 'H', ['not', 'A'], 'D'], ['or', ['not', 'F'], 'F', ['not', 'H'], ['not', 'G'], ['not', 'A'], 'D'], ['or', 'E', 'F', 'G', 'H', ['not', 'B'], 'D'], ['or', 'E', 'F', 'G', ['not', 'G'], ['not', 'B'], 'D'], ['or', 'E', 'F', ['not', 'H'], 'H', ['not', 'B'], 'D'], ['or', 'E', ['not', 'E'], 'G', 'H', ['not', 'B'], 'D'], ['or', 'E', ['not', 'E'], 'G', ['not', 'G'], ['not', 'B'], 'D'], ['or', 'E', ['not', 'E'], ['not', 'H'], 'H', ['not', 'B'], 'D'], ['or', 'E', 'F', ['not', 'H'], ['not', 'G'], ['not', 'B'], 'D'], ['or', 'E', ['not', 'E'], ['not', 'H'], ['not', 'G'], ['not', 'B'], 'D'], ['or', ['not', 'F'], 'F', 'G', 'H', ['not', 'B'], 'D'], ['or', ['not', 'F'], 'F', 'G', ['not', 'G'], ['not', 'B'], 'D'], ['or', ['not', 'F'], 'F', ['not', 'H'], 'H', ['not', 'B'], 'D'], ['or', ['not', 'F'], 'F', ['not', 'H'], ['not', 'G'], ['not', 'B'], 'D'], ['or', 'E', 'F', 'I', 'J', ['not', 'A'], 'D'], ['or', 'E', 'F', 'I', ['not', 'I'], ['not', 'A'], 'D'], ['or', 'E', 'F', ['not', 'J'], 'J', ['not', 'A'], 'D'], ['or', 'E', ['not', 'E'], 'I', 'J', ['not', 'A'], 'D'], ['or', 'E', ['not', 'E'], 'I', ['not', 'I'], ['not', 'A'], 'D'], ['or', 'E', ['not', 'E'], ['not', 'J'], 'J', ['not', 'A'], 'D'], ['or', 'E', 'F', ['not', 'J'], ['not', 'I'], ['not', 'A'], 'D'], ['or', 'E', ['not', 'E'], ['not', 'J'], ['not', 'I'], ['not', 'A'], 'D'], ['or', ['not', 'F'], 'F', 'I', 'J', ['not', 'A'], 'D'], ['or', ['not', 'F'], 'F', 'I', ['not', 'I'], ['not', 'A'], 'D'], ['or', ['not', 'F'], 'F', ['not', 'J'], 'J', ['not', 'A'], 'D'], ['or', ['not', 'F'], 'F', ['not', 'J'], ['not', 'I'], ['not', 'A'], 'D'], ['or', 'E', 'F', 'I', 'J', ['not', 'B'], 'D'], ['or', 'E', 'F', 'I', ['not', 'I'], ['not', 'B'], 'D'], ['or', 'E', 'F', ['not', 'J'], 'J', ['not', 'B'], 'D'], ['or', 'E', ['not', 'E'], 'I', 'J', ['not', 'B'], 'D'], ['or', 'E', ['not', 'E'], 'I', ['not', 'I'], ['not', 'B'], 'D'], ['or', 'E', ['not', 'E'], ['not', 'J'], 'J', ['not', 'B'], 'D'], ['or', 'E', 'F', ['not', 'J'], ['not', 'I'], ['not', 'B'], 'D'], ['or', 'E', ['not', 'E'], ['not', 'J'], ['not', 'I'], ['not', 'B'], 'D'], ['or', ['not', 'F'], 'F', 'I', 'J', ['not', 'B'], 'D'], ['or', ['not', 'F'], 'F', 'I', ['not', 'I'], ['not', 'B'], 'D'], ['or', ['not', 'F'], 'F', ['not', 'J'], 'J', ['not', 'B'], 'D'], ['or', ['not', 'F'], 'F', ['not', 'J'], ['not', 'I'], ['not', 'B'], 'D'], ['or', 'E', 'F', 'G', 'H', ['not', 'C'], 'D'], ['or', 'E', 'F', 'G', ['not', 'G'], ['not', 'C'], 'D'], ['or', 'E', 'F', ['not', 'H'], 'H', ['not', 'C'], 'D'], ['or', 'E', ['not', 'E'], 'G', 'H', ['not', 'C'], 'D'], ['or', 'E', ['not', 'E'], 'G', ['not', 'G'], ['not', 'C'], 'D'], ['or', 'E', ['not', 'E'], ['not', 'H'], 'H', ['not', 'C'], 'D'], ['or', 'E', 'F', ['not', 'H'], ['not', 'G'], ['not', 'C'], 'D'], ['or', 'E', ['not', 'E'], ['not', 'H'], ['not', 'G'], ['not', 'C'], 'D'], ['or', ['not', 'F'], 'F', 'G', 'H', ['not', 'C'], 'D'], ['or', ['not', 'F'], 'F', 'G', ['not', 'G'], ['not', 'C'], 'D'], ['or', ['not', 'F'], 'F', ['not', 'H'], 'H', ['not', 'C'], 'D'], ['or', ['not', 'F'], 'F', ['not', 'H'], ['not', 'G'], ['not', 'C'], 'D'], ['or', 'E', 'F', 'I', 'J', ['not', 'C'], 'D'], ['or', 'E', 'F', 'I', ['not', 'I'], ['not', 'C'], 'D'], ['or', 'E', 'F', ['not', 'J'], 'J', ['not', 'C'], 'D'], ['or', 'E', ['not', 'E'], 'I', 'J', ['not', 'C'], 'D'], ['or', 'E', ['not', 'E'], 'I', ['not', 'I'], ['not', 'C'], 'D'], ['or', 'E', ['not', 'E'], ['not', 'J'], 'J', ['not', 'C'], 'D'], ['or', 'E', 'F', ['not', 'J'], ['not', 'I'], ['not', 'C'], 'D'], ['or', 'E', ['not', 'E'], ['not', 'J'], ['not', 'I'], ['not', 'C'], 'D'], ['or', ['not', 'F'], 'F', 'I', 'J', ['not', 'C'], 'D'], ['or', ['not', 'F'], 'F', 'I', ['not', 'I'], ['not', 'C'], 'D'], ['or', ['not', 'F'], 'F', ['not', 'J'], 'J', ['not', 'C'], 'D'], ['or', ['not', 'F'], 'F', ['not', 'J'], ['not', 'I'], ['not', 'C'], 'D'], ['or', ['not', 'F'], ['not', 'E'], 'G', 'H', ['not', 'A'], 'D'], ['or', ['not', 'F'], ['not', 'E'], 'G', ['not', 'G'], ['not', 'A'], 'D'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'H'], 'H', ['not', 'A'], 'D'], ['or', ['not', 'F'], ['not', 'E'], 'G', 'H', ['not', 'B'], 'D'], ['or', ['not', 'F'], ['not', 'E'], 'G', ['not', 'G'], ['not', 'B'], 'D'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'H'], 'H', ['not', 'B'], 'D'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'H'], ['not', 'G'], ['not', 'A'], 'D'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'H'], ['not', 'G'], ['not', 'B'], 'D'], ['or', ['not', 'F'], ['not', 'E'], 'G', 'H', ['not', 'C'], 'D'], ['or', ['not', 'F'], ['not', 'E'], 'G', ['not', 'G'], ['not', 'C'], 'D'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'H'], 'H', ['not', 'C'], 'D'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'H'], ['not', 'G'], ['not', 'C'], 'D'], ['or', 'E', 'F', 'G', 'H', ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', 'F', 'G', ['not', 'G'], ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', 'F', ['not', 'H'], 'H', ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', ['not', 'E'], 'G', 'H', ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', ['not', 'E'], 'G', ['not', 'G'], ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', ['not', 'E'], ['not', 'H'], 'H', ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', 'F', ['not', 'H'], ['not', 'G'], ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', ['not', 'E'], ['not', 'H'], ['not', 'G'], ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], 'F', 'G', 'H', ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], 'F', 'G', ['not', 'G'], ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], 'F', ['not', 'H'], 'H', ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], 'F', ['not', 'H'], ['not', 'G'], ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', 'F', 'I', 'J', ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', 'F', 'I', ['not', 'I'], ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', 'F', ['not', 'J'], 'J', ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', ['not', 'E'], 'I', 'J', ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', ['not', 'E'], 'I', ['not', 'I'], ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', ['not', 'E'], ['not', 'J'], 'J', ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', 'F', ['not', 'J'], ['not', 'I'], ['not', 'D'], 'A', 'B', 'C'], ['or', 'E', ['not', 'E'], ['not', 'J'], ['not', 'I'], ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], 'F', 'I', 'J', ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], 'F', 'I', ['not', 'I'], ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], 'F', ['not', 'J'], 'J', ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], 'F', ['not', 'J'], ['not', 'I'], ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], ['not', 'E'], 'G', 'H', ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], ['not', 'E'], 'G', ['not', 'G'], ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'H'], 'H', ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'H'], ['not', 'G'], ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], ['not', 'E'], 'I', 'J', ['not', 'A'], 'D'], ['or', ['not', 'F'], ['not', 'E'], 'I', ['not', 'I'], ['not', 'A'], 'D'], ['or', ['not', 'F'], ['not', 'E'], 'I', 'J', ['not', 'B'], 'D'], ['or', ['not', 'F'], ['not', 'E'], 'I', ['not', 'I'], ['not', 'B'], 'D'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'J'], 'J', ['not', 'A'], 'D'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'J'], 'J', ['not', 'B'], 'D'], ['or', ['not', 'F'], ['not', 'E'], 'I', 'J', ['not', 'C'], 'D'], ['or', ['not', 'F'], ['not', 'E'], 'I', ['not', 'I'], ['not', 'C'], 'D'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'J'], 'J', ['not', 'C'], 'D'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'J'], ['not', 'I'], ['not', 'A'], 'D'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'J'], ['not', 'I'], ['not', 'B'], 'D'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'J'], ['not', 'I'], ['not', 'C'], 'D'], ['or', ['not', 'F'], ['not', 'E'], 'I', 'J', ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], ['not', 'E'], 'I', ['not', 'I'], ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'J'], 'J', ['not', 'D'], 'A', 'B', 'C'], ['or', ['not', 'F'], ['not', 'E'], ['not', 'J'], ['not', 'I'], ['not', 'D'], 'A', 'B', 'C']]
    dpllwrapper(sentence)