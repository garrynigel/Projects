#
# Import package argv from system for 
# using parameters
from sys import argv
import itertools
from collections import defaultdict
from os.path import os
from os.path import basename
#
script,mode,file_name = argv
#
#
#  This function build the dictionaries for each file
#  disease_dict : Stores the list of symptoms for each disease
#  disease_prob : The priori probabilities for each disease
#  disease_domain : Ordered domain of diseases as appeared in file
#  symp_dis_prsnt_prob : Symptom probabilities when the disease is present
#  symp_dis_absnt_prob : Symptom probabilities when the disease is absent
#  
#
def build_diseases(file_inst):
    #
    #
    disease_info = file_inst.readline()
    disease_info = disease_info.split()
    #
    #
    # Store the domain of diseases
    disease_domain.append(disease_info[0])
    #
    # store the disease name and 
    # the priori probability
    #
    disease_prob[disease_info[0]] = disease_info[2]
    #
    # Store the list of symptoms for each disease
    #
    symptom_lst = file_inst.readline()
    symptom_lst = eval(symptom_lst)
    disease_dict[disease_info[0]] = symptom_lst
    #
    # Store the probabilites for the symptoms
    # When the disease is present
    # 
    prob = file_inst.readline()
    prob = eval(prob)
    for i in range(len(symptom_lst)):
        symp_dis_prsnt_prob[symptom_lst[i]] = prob[i]
    #
    # Store the probabilities for the symptoms
    # When the disease is absent
    #
    prob = file_inst.readline()
    prob = eval(prob)
    for i in range(len(symptom_lst)):
        symp_dis_absnt_prob[symptom_lst[i]] = prob[i]
#
#
def has_disease_calc(finding,symptoms):
    #symptoms = disease_dict[disease]
    symptom_prob_present = []
    symptom_prob_absent = []
    for i in range(len(symptoms)):
        if finding[i] == 'T':
            symptom_prob_present.append(float(symp_dis_prsnt_prob[symptoms[i]]))
        elif finding[i] == 'F':
            symptom_prob_present.append(1-float(symp_dis_prsnt_prob[symptoms[i]]))
        else:
            symptom_prob_present.append(1.0)
    for i in range(len(symptoms)):
        if finding[i] == 'T':
            symptom_prob_absent.append(float(symp_dis_absnt_prob[symptoms[i]]))
        elif finding[i] == 'F':
            symptom_prob_absent.append(1-float(symp_dis_absnt_prob[symptoms[i]]))
        else:
            symptom_prob_absent.append(1.0)
    #print 'Symptom prompt Absent ',symptom_prob_absent
    #print 'Symptom prompt Present ',symptom_prob_present


    numerator = float(reduce(lambda x,y : x*y , symptom_prob_present))
    denominator = float(reduce(lambda x,y : x*y , symptom_prob_absent))
    
    #print 'disease',disease
    #print 'Numerator ',numerator 
    #print 'Denominator',denominator

    #prob = round(numerator/(numerator+denominator),4)

    return numerator,denominator
#
#
def create_possible_findings(undecided_symptoms):
    possible_finds = []
    if len(undecided_symptoms) == 0:
        return None
    else:
        possible_finds = list(itertools.product(['T','F'],repeat = len(undecided_symptoms)))
    return possible_finds

#
# Calculate the Undecided symptom difference f
# for the truth and the false value
# 
def calc_diff(symptom,orig_prob,orig_num,orig_den):
    t_num,t_den = has_disease_calc(['T'],[symptom])
    t_prob = 0.0
    f_prob = 0.0
    t_prob = (orig_num*t_num)/((orig_num*t_num)+(orig_den*t_den))

    print 't_prob',t_prob
    t_prob = t_prob - orig_prob
    print 'Symptom',symptom
    print 'tnum',t_num
    print 'tden',t_den
    print 'OrigNum',orig_num
    print 'OrigNum',orig_den

    f_num,f_den = has_disease_calc(['F'],[symptom])
    f_prob = (orig_num*f_num)/((orig_num*f_num)+(orig_den*f_den))
    print 'f_prob',f_prob
    f_prob = f_prob - orig_prob
    print 'Symptom',symptom
    print 'tnum',f_num
    print 'tden',f_den

    return t_prob,f_prob
#
#
# Write output to file
#
def write_file(inference1,inference2,inference3,op_file,patient):
    op_file.write(str(patient)+':')
    op_file.write('\n')
    op_file.write(str(inference1))
    op_file.write('\n')
    op_file.write(str(inference2))
    op_file.write('\n')
    op_file.write(str(inference3))
    op_file.write('\n')
#
#
#
# Function parse disease parses through each disease
# and returns the probabilites of each disease for
# each patient
#
#
def parse_disease(patient_key):
    #
    # obtain the findings for each patient
    #
    disease_findings = patient_findings[patient_key]
    # 
    # Send the disease and the respective finding for calcuation
    # to has_disease_calc
    #
    #
    disprob1 = {}
    disprob2 = {}
    disprob3 = {}
    for i in range(len(disease_domain)):
        undecided_symptoms =[]
        disease = disease_domain[i]
        symptoms = disease_dict[disease]
        orig_num,orig_den = has_disease_calc(disease_findings[i],symptoms)

        orig_num = float(orig_num * float(disease_prob[disease]))
        orig_den = float(orig_den * (1-float(disease_prob[disease])))
        prob = orig_num/(orig_num+orig_den)
        disprob1[disease_domain[i]] = '{:.4f}'.format(round(prob,4))
        #
        # Assign min_prob and max_prob
        #
        # Find all undecided symptoms
        #
        findings = disease_findings[i]
        for i in range(len(findings)):
            if findings[i] == 'U':
                undecided_symptoms.append(symptoms[i])
        #
        #
        all_poss_findings = create_possible_findings(undecided_symptoms)
        prob_list =[]
        prob_list.append(prob)
        #
        # Handle if undecided none
        # def undecided_handler
        #
        #
        if all_poss_findings != None:
            for i in range(len(all_poss_findings)):
                numerator,denominator = has_disease_calc(all_poss_findings[i],undecided_symptoms)
    
                find_prob = round((orig_num*numerator)/((orig_num*numerator)+(orig_den*denominator)),4)
                prob_list.append(find_prob)
        min_prob = min(prob_list)
        max_prob = max(prob_list)
        disprob2[disease] = ['{:.4f}'.format(min_prob),'{:.4f}'.format(max_prob)]
        #
        #
        # Build the dictionaries for truth and false values for each
        # undecided symptom and calculate difference from original
        # probability to give increase or decrease of probability
        #
        diff_prob = {}
        t_prob = None
        f_prob = None
        for i in range(len(undecided_symptoms)):
            t_prob,f_prob = calc_diff(undecided_symptoms[i],prob,orig_num,orig_den)
            diff_prob[undecided_symptoms[i]] = list([t_prob,f_prob])
        #print prob
        #print orig_num
        #print orig_den
        print 'diff_prob',diff_prob
        max_diff = 0.0
        min_diff = 0.0
        test_max = 'N'
        test_min = 'N'
        sym_max  = None
        sym_min  = None
        for key in diff_prob:
            diff_list = diff_prob[key]
            if diff_list[0] == max_diff:
                if sym_max > key:
                    sym_max = key
                    test_max = 'T'
            if diff_list[1] == max_diff:
                if sym_max > key:
                    sym_max = key
                    test_max = 'F'
            if diff_list[0] == min_diff:
                if sym_min > key:
                    sym_min = key
                    test_min = 'T'
            if diff_list[1] == min_diff:
                if sym_min > key:
                    sym_min = key
                    test_min = 'F'
            if diff_list[0] > max_diff:
                max_diff = diff_list[0]
                test_max = 'T'
                sym_max = key
            if diff_list[0] < min_diff:
                min_diff = diff_list[0]
                test_min = 'T'
                sym_min = key
            if diff_list[1] > max_diff:
                max_diff = diff_list[1]
                test_max = 'F'
                sym_max = key
            if diff_list[1] < min_diff:
                min_diff = diff_list[1]
                test_min = 'F'
                sym_min = key

        disprob3_value = list([sym_max,test_max,sym_min,test_min])
        disprob3[disease] = disprob3_value
    #print patient_key
    #print '1st Disprob',disprob1
    #print '2nd Disprob',disprob2
    #print '3rd Disprob',disprob3
    #
    #
    return disprob1,disprob2,disprob3
#
def wrapper(file_name):
    #******************************
    # Open file
    #******************************
    #
    file_inst = open(file_name,'r')
    #
    #
    # Reading the first line in the file for the counts
    #
    #
    line_cnts = file_inst.readline()
    line_cnts = line_cnts.split()
    disease_cnt = int(line_cnts[0])
    patient_cnt = int(line_cnts[1])
    #
    #
    # Create output file
    #
    file_name = basename(file_name)
    op_file_name = os.path.splitext(file_name)[0]
    op_file_name = op_file_name+'_inference.txt'
    op_inst = open(op_file_name,'w')
    #
    #
    #*******************************
    # Build the disease probabilties
    #*******************************
    #
    # Declare the global dictionaries
    #
    #
    global disease_dict
    global symp_dis_prsnt_prob
    global symp_dis_absnt_prob
    global disease_prob
    global patient_findings
    global disease_domain
    #global numerator
    #global denominator
    
    disease_domain =[]
    disease_dict = {}
    symp_dis_prsnt_prob = {}
    symp_dis_absnt_prob = {}
    disease_prob = {}
    patient_findings = {}
    inference1= {}
    inference2= {}
    inference3= {}

    #numerator = 1.0
    #denominator = 1.0
    #
    # Build the dictionaries for each
    # of the diseases and the respective symptoms
    #
    for i in range(disease_cnt):
        build_diseases(file_inst)
    #
    #
    #
    # Obtain findings for each of the patients
    #
    #
    for i in range(1,patient_cnt+1):
        key = 'Patient-'+str(i)
        patient_findings.setdefault(key,[])
        for j in range(disease_cnt):
            findings = file_inst.readline()
            findings = eval(findings)
            patient_findings[key].append(findings)
    #
    #
    #
    # Build dictionaries for each question for each patient
    # For every patient calculate the probability he has
    # the disease
    #
    for i in range(1,patient_cnt+1):
        key = 'Patient-'+str(i)
        has_disease = {}
        inference1,inference2,inference3 = parse_disease(key)
        write_file(inference1,inference2,inference3,op_inst,key) 
    #print'Disease Dict',disease_dict
    #print '\n'
    #print'Symptoms when disease is present',symp_dis_prsnt_prob
    #print '\n'
    #print'Symptoms when disease is absent',symp_dis_absnt_prob
    #print '\n'
    #print'Disease Priori Probabilities',disease_prob
    #print '\n'
    #print'Patient findings',patient_findings 
    #print '\n'
    #print'Disease Domain',disease_domain 
    #print '\n'
    op_inst.close()
    file_inst.close()
          
           

#*****************************
#
# Main 
#
#*****************************

wrapper(file_name)
