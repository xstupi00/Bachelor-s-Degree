import sys, getopt, re, copy, operator, codecs, collections, os
import xml.etree.ElementTree as ET 

inst_dict = {
    ('CREATEFRAME', 'PUSHFRAME', 'RETURN', 'BREAK', 'POPFRAME', 'ADDS', 'SUBS', 'MULS', 'IDIVS', 'LTS', 'GTS', 'EQS', 'ANDS', 'ORS', 'NOTS', 'INT2CHARS', 'STRI2INTS', 'CLEARS'): [0],
    ('CALL', 'LABEL', 'JUMP', 'JUMPIFEQS', 'JUMPIFNEQS'): [1, 'label'],
    ('PUSHS', 'DPRINT', 'WRITE'): [1, 'symb'],
    ('POPS', 'DEFVAR'): [1, 'var'],
    ('ADD', 'SUB', 'MUL', 'IDIV', 'LT', 'GT', 'EQ', 'AND', 'OR', 'STRI2INT', 'CONCAT', 'GETCHAR', 'SETCHAR'): [3, 'var', 'symb', 'symb'],
    ('NOT', 'MOVE', 'INT2CHAR', 'STRLEN', 'TYPE'): [2, 'var', 'symb'],
    ('READ'): [2, 'var', 'type'],
    ('JUMPIFEQ', 'JUMPIFNEQ'): [3, 'label', 'symb', 'symb'],
}

frame_stack = list()
call_stack = list()
data_stack = list()
GF = dict()
labels_dict = dict()
TF = None
LF = None
DEBUG = False
insts = 0
vars = 0

def print_help():
    """Print users help message at wrong combinations of arguments of
    command line or when users relevent argument for this message

    Return:
        exit: function exits the program with relevant return code

    """

    print('python3.6 interpret.py --source <xml_file>', file=sys.stderr)
    sys.exit(0)

def print_err(err_code):
    """Print error messages when error state in the program was occurred

    Arguments:
        err_code(int): code of the occurred error
    Return:
        exit: function exits the program with err_code
    """

    if err_code == 10:
        print("Missing script paramater.", file=sys.stderr)
    elif err_code == 11:
        print("Error at opening input file.", file=sys.stderr)
    elif err_code == 12:
        print("Errot at opening output file.", file=sys.stderr)
    elif err_code == 31:
        print('Invalid XML input file format', file=sys.stderr)
    elif err_code == 32:
        print('Error of lexical or syntactic analysis of text elements and attributes in input XMLFile', file=sys.stderr)
    elif err_code == 52:
        print('Error in semantic input code at checks in IPPcode18.', file=sys.stderr)
    elif err_code == 53:
        print('Wrong type of operands.', file=sys.stderr)
    elif err_code == 54:
        print('Access to a non-existent variable', file=sys.stderr)
    elif err_code == 55:
        print("Frame does not exists.", file=sys.stderr)
    elif err_code == 56:
        print('Missing value (in variable, on stack, or in the call stack)', file=sys.stderr)
    elif err_code == 57:
        print('Zero division', file=sys.stderr)
    elif err_code == 58:
        print('Wrong work with string', file=sys.stderr)
    sys.exit(err_code)

def check_attrib(attrib, key, flag=None):
    """Control of existence of the given attribut from the XMLFile

    Arguments:
        attrib(XMLElement): the element which should have contains given attribut
        key(string): the key of the required attribut
        flag(bool): resolution of fatal error from nonfatal
    Raises:
        KeyError: if specified attribut with specified key not exists
    Return:
        bool: existence of the given attribut with specified key

    """

    if flag is None:
        try:
            _ = attrib[key]
        except KeyError:
            print_err(31) # its structure?
    else:
        try:
            _ = attrib[key]
            return True
        except KeyError:
            return False
    
def check_instr(instruction, opcode):
    """The syntactic and lexical control of instruction, respectively its arguments and
    its types and contains in the text element. Control is executed with set of Regular expressions.

    Arguments:
        instruction(XMLElement): the instruction with the needed data
        opcode(ListItem): key to the instruction dictionary
    Return:
        exit: functions exits the program in the case when some error was occurred

    """

    if instruction.tag[:len(instruction.tag)-1].lower() != 'arg':
        print_err(31) # its structure?
    if not(0 < int(instruction.tag[-1]) < len(inst_dict[opcode])):
        print_err(31)
    arg_num = int(instruction.tag[-1])
    check_attrib(instruction.attrib, 'type')
    if (len(instruction.attrib) > 1):
        print_err(31)
    if inst_dict[opcode][arg_num] == 'symb' and (instruction.attrib['type'] in ('bool', 'int', 'string') or instruction.attrib['type'] in ('var')):
        if instruction.text is None and instruction.attrib['type'] in ('bool', 'int', 'var'):
            print_err(32)
        if instruction.text is None and instruction.attrib['type'] in ('string'):
            instruction.text = ''
        if instruction.attrib['type'] in ('int')    and re.match(r'^(-|\+)?[0-9]+$', instruction.text) is None\
        or instruction.attrib['type'] in ('bool')   and re.match(r'^(false|true)$', instruction.text) is None\
        or instruction.attrib['type'] in ('string') and re.match(r'^(([^\x00-\x1F\x7F\xA0\#])+|(\x5C[0-9]{3})+|($))$', instruction.text) is None\
        or instruction.attrib['type'] in ('var')    and re.match(r'^(LF|TF|GF)@([a-zA-Z]|_|-|\$|&|%|\*)([a-zA-Z0-9]|_|-|\$|&|%|\*)*$', instruction.text) is None:
                print_err(32)
    elif inst_dict[opcode][arg_num] == 'var' and instruction.attrib['type'] in ('var'):
        if instruction.text is None:
            print_err(32)
        if re.match(r'^(LF|TF|GF)@([a-zA-Z]|_|-|\$|&|%|\*)([a-zA-Z0-9]|_|-|\$|&|%|\*)*$', instruction.text) is None:
            print_err(32)
    elif inst_dict[opcode][arg_num] == 'label' and instruction.attrib['type'] in ('label'):
        if instruction.text is None:
            print_err(32)
        if re.match(r'^([a-zA-Z]|_|-|\$|&|%|\*)([a-zA-Z0-9]|_|-|\$|&|%|\*)*$', instruction.text) is None:
            print_err(32)
    elif inst_dict[opcode][arg_num] == 'type' and instruction.attrib['type'] in ('type'):
        if instruction.text is None:
            print_err(32)
        if re.match(r'^\s*(int|bool|string)(\s*)$', instruction.text) is None:
            print_err(32)
    else:
        print_err(32)
    
def transform_string(string):
    """Transform the string, which can contains the escape sequences and replace them 
    for the relevant printable chars

    Arguments:
        string(string): string to transformation
    Return:
        string: the string without escape sequences

    """

    i = 0
    while i < len(string):
        if string[i] in ('\\'):
            c = int(string[i+1:i+4])
            up = i + 4
            if up >= len(string):
                up = len(string) 
            string = string[0:i] + chr(c) + string[up:]
        i += 1
    return string

def back_transform(string):
    """Tranform the string, which can contains not allowed printable chars to the 
    string with escape sequences

    Arguments:
        string(string): string to tranformation
    Return:
        string: the string with escape sequences

    """

    i = 0
    while i < len(string):
        c = ord(string[i])
        if 0 <= c <= 32 or c == 35 or c == 92:
            string = string[0:i] + '\\0' + str(c) + string[i+1:]
        i+=1
    return string
    
def check_root(xml_root):
    """The control of root element of the XMLFile. Control is performed at element attribut
    and its text element. Have to fulfilled all the required properties of this root element.

    Arguments:
        xml_root(XMLElement): the root element for control
    Return:
        exit: the function exit the program, when some error was occurred

    """

    if xml_root.tag.lower() != 'program':
        print_err(31)
    check_attrib(xml_root.attrib, 'language')
    if xml_root.attrib['language'].lower() != 'ippcode18':
        print_err(32)
    if (len(xml_root.attrib) == 3):
        flag_n = check_attrib(xml_root.attrib, 'name', True)
        flag_d = check_attrib(xml_root.attrib, 'description', True)
        if flag_n == False or flag_d == False:
            print_err(31)
    elif (len(xml_root.attrib) == 2):
        flag = check_attrib(xml_root.attrib, 'name', True)
        if flag == False:
            flag = check_attrib(xml_root.attrib, 'description', True)
            if flag == False:
                print_err(31)
    elif (len(xml_root.attrib) > 3):
        print_err(31)


def parse_xml_file(xml_namefile):
    """The brain of the whole program, because executing the basic logic of program. Check the correctness
    of all element and its text part. Divides all the work other methods according to actual state.
    Representing the general state of Finite State Machine (see documentation for nearest information).

    Arguments:
        xml_namefile(FileHandle): the path to the input XMLFile with input code
    Return:
        exit: function exit only in the case, when all instructions will be executing succesfully

    """

    global GF, TF, LF, frame_stack, labels_dict, insts, data_stack
    try:
        xml_tree = ET.parse(xml_namefile)
    except ET.ParseError:
        print_err(31)
    xml_root = xml_tree.getroot()
    
    check_root(xml_root)

    order_array = list()
    for instruction in xml_root.findall('instruction'):
        check_attrib(instruction.attrib, 'opcode')
        check_attrib(instruction.attrib, 'order')
        if (len(instruction.attrib) > 2):
            print_err(31)
        try:
            order_array.append(int(instruction.attrib['order']))
        except ValueError:
            print_err(32)
        if instruction.attrib['opcode'] in ('LABEL'):   
            if labels_dict.get(instruction[0].text) is None:
                labels_dict[instruction[0].text] = int(instruction.attrib['order'])
            else:
                print_err(52)
    order_array.sort()

    if len(order_array) != len(xml_root):
        print_err(31)

    inst_reader = 0
    if not order_array and len(xml_root):
        print_err(31)
    while inst_reader != len(xml_root):
        root_index = 0 
        while order_array[inst_reader] != int(xml_root[root_index].attrib['order']):
            root_index += 1
        inst_reader += 1

        elem = xml_root[root_index]

        if len(elem) > 3:
            print_err(31)
        success = False
        cnt_arg = -1
        arg_order = list()
        for key in inst_dict.keys():
            if elem.attrib['opcode'] in key:
                if len(elem) != inst_dict[key][0]:
                    print_err(31) # its structure?
                success = True
                cnt_arg = inst_dict[key][0]
                for subelem in elem:
                    try:
                        arg_order.append(int(subelem.tag[-1]))
                    except ValueError:
                        print_err(31)
                    [print_err(31) for x, y in collections.Counter(arg_order).items() if y > 1]
                    check_instr(subelem, key)
        if not success:
            print_err(32)
        
        inst_name = elem.attrib['opcode'].lower()
        if cnt_arg == 0:
            if inst_name in ('break'):
                break_()
            elif inst_name in ('return'):
                inst_reader = return_()
            elif inst_name in ('adds', 'subs', 'muls', 'idivs'):
                arithmetic_S(inst_name)
            elif inst_name in ('lts', 'gts', 'eqs'):
                compare_S(inst_name)
            elif inst_name in ('ands', 'ors'):
                logic_S(inst_name)
            elif inst_name in ('nots'):
                not_S()
            elif inst_name in ('int2chars'):
                int2char_S()
            elif inst_name in ('stri2ints'):
                stri2int_S()
            elif inst_name in ('clears'):
                data_stack = list()
            else:
                eval(inst_name)()
        elif cnt_arg == 1:
            if inst_name in ('call'):
                inst_reader = eval(inst_name)(elem[0], inst_reader, order_array)
            elif inst_name in ('jump'):
                inst_reader = call(elem[0], inst_reader, order_array, True)
            elif inst_name in ('jumpifeqs', 'jumpifneqs'):
                inst_reader = jumps_S(inst_name, elem[0], inst_reader, order_array)
            else:   
                eval(inst_name)(elem[0])
        elif cnt_arg == 2:
            first_arg = arg_order.index(1)
            second_arg = arg_order.index(2)
            if inst_name in ('not'):
                why_not(elem[first_arg], elem[second_arg])
            elif inst_name in ('type'):
                type_(elem[first_arg], elem[second_arg])
            else:
                eval(inst_name)(elem[first_arg], elem[second_arg])
        elif cnt_arg == 3:
            first_arg = arg_order.index(1)
            second_arg = arg_order.index(2)
            third_arg = arg_order.index(3)
            if inst_name in ('add', 'sub', 'mul', 'idiv'):
                arithmetic(elem[first_arg], elem[second_arg], elem[third_arg], inst_name)
            elif inst_name in ('lt', 'gt', 'eq'):
                compare(elem[first_arg], elem[second_arg], elem[third_arg], inst_name)
            elif inst_name in ('and', 'or'):
                logic(elem[first_arg], elem[second_arg], elem[third_arg], inst_name)
            elif inst_name in ('jumpifeq', 'jumpifneq'):
                inst_reader = jumps(inst_name, elem[first_arg], elem[second_arg], elem[third_arg], inst_reader, order_array) 
            else:
                eval(inst_name)(elem[first_arg], elem[second_arg], elem[third_arg])
        check_vars()
        insts += 1

def check_vars():
    """Realization of extension, which count the defined variables in the all frames
    during the executing instructions. Function count the actual counts of defined
    variables and compared with the maximal count.

    """
    global LF, TF, GF, vars
    cnt = 0
    dict_list = {0: 'LF', 1: 'GF', 2: 'TF'}
    for _, dict_name in dict_list.items():
        if eval(dict_name) is not None:
            for _, value in eval(dict_name).items():
                if value[0] is not None:
                    cnt += 1
    vars = max(cnt, vars)

def check_exist_frame(name):
    """Check existence the frame. When the frame not exists, error state is called.

    Arguments:
        name(string): name of the frame
    """

    global GF, TF, LF
    if eval(name) is None:
        print_err(55)

def check_exist_var(frame, var):
    """Check existence the variable. When the variable not exists, error state is called.

    Arguments:
        name(string): name of the variable
    """

    global GF, TF, LF
    if eval(frame).get(var) is None:
        print_err(54)

def check_undef_var(frame, var):
    """Check if the variable was defined and have the concrete value and type.
    In the case, when the variable have been not defined, only declared, error
    state is called.

    Arguments:
        frame(string): name of the frame where the variable have to exists
        var(string): name of the variable
    """

    global GF, TF, LF
    if eval(frame)[var][0] is None:
        print_err(56)

def move(arg1, arg2):
    """The function executing the logic of instruction MOVE.

    Arguments:
        arg1(XMLElement): the target variable
        arg2(XMLElement): the symbol (variable or constant) which contains
                          the value, which will be move to the arg1
    """

    global TF, LF, GF
    if arg2.attrib['type'] in ('int', 'string', 'bool'):
        if arg1.text.split('@')[0] in ('LF', 'GF', 'TF'):
            check_exist_frame(arg1.text.split('@')[0])
            check_exist_var(arg1.text.split('@')[0], arg1.text.split('@')[1])
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = arg2.text
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = arg2.attrib['type']
    elif arg2.attrib['type'] in ('var'):
        check_exist_frame(arg1.text.split('@')[0])
        check_exist_frame(arg2.text.split('@')[0])
        check_exist_var(arg2.text.split('@')[0], arg2.text.split('@')[1])
        if arg1.text.split('@')[0] in ('LF', 'GF', 'TF'):
            check_exist_var(arg1.text.split('@')[0], arg1.text.split('@')[1])
            check_undef_var(arg2.text.split('@')[0], arg2.text.split('@')[1])
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][0]
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][1]

def createframe():
    """The function executing the logic of instruction CREATEFRAME.
    Function creating the TEMPORARY FRAME, which will be after the creating empty.

    """

    global TF
    TF = dict()

def pushframe():
    """The function executing the logic of instruction PUSHFRAME.
    Function defined LOCAL FRAME and it will happen copy of the TEMPORARY FRAME.
    TEMPORARY FRAME will be empty after the execution.

    """

    global TF, LF, frame_stack
    check_exist_frame('TF')
    LF = copy.deepcopy(TF)
    frame_stack.append(LF)
    TF = None

def popframe():
    """The function executing the logic of instruction POPFRAME.
    Function defined TEMPORARY FRAME and it will happen copy of the LOCAL FRAME, respectively
    will obtaining the TOP from the stack of local frames.
    LOCAL FRAME will be contains new TOP of the stack of local frames, if it not empty, else
    will be undefined.
    
    """

    global TF, LF, frame_stack
    if not frame_stack:
        print_err(55)
    TF = frame_stack.pop()
    if not frame_stack:
        LF = None
    else:
        LF = frame_stack[-1]
    
def defvar(arg):
    """The function executing the logic of instruction DEFVAR.
    The variable will be only created (declared) and for work with them must be initialized.

    Arguments:
        arg(XMLElement): the variable to creating on the given frame
    """

    global TF, LF, GF
    if arg.text.split('@')[0] in ('LF', 'TF'):
        check_exist_frame(arg.text.split('@')[0])
        eval(arg.text.split('@')[0])[arg.text.split('@')[1]] = {0: None, 1: None}
    elif arg.text.split('@')[0] in ('GF'):
        eval(arg.text.split('@')[0])[arg.text.split('@')[1]] = {0: None, 1: None}

def call(label, inst_reader, order_array, push=False):
    """The function executing the logic of instruction CALL.
    Realization of call the function, remember the actual executing instruction

    Arguments:
        label(XMLElement): label for jump, place where will be next instruction to execution
        inst_reade(int): index to array with instruction order
        order_array(list): list with ordered instruction
        push(bool): temporary flag for different type of instruction realized the same logic
    Return:
        inst_reader: index to list with instruction or the next executing instruction

    """

    global call_stack, labels_dict
    if push == False:
        call_stack.append(inst_reader)
    if labels_dict.get(label.text) is None:
        print_err(52)
    else:
        num_order = labels_dict.get(label.text)
        return order_array.index(num_order)

def return_():
    """The function executing the logic of instruction RETURN.
    Function realized the return from the called part of the program

    Return:
        inst_reader: index to list with instruction or the next executing instruction
        
    """

    global call_stack
    if not call_stack:
        print_err(56)
    return call_stack.pop()

def pushs(arg):
    """The function executing the logic of instruction PUSHS.
    Stored the symbol (cosntant or variable (its value and type)) to the data stack

    Arguments:
        arg(XMLElement): the variable or constant to stored to the stack
    """

    global data_stack
    if arg.attrib['type'] in ('int', 'string', 'bool'):
        data_stack.append([arg.text, arg.attrib['type']])
    elif arg.attrib['type'] in ('var'):
        check_exist_frame(arg.text.split('@')[0])
        check_exist_var(arg.text.split('@')[0], arg.text.split('@')[1])
        check_undef_var(arg.text.split('@')[0], arg.text.split('@')[1])
        data_stack.append([eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0], eval(arg.text.split('@')[0])[arg.text.split('@')[1]][1]])

def pops(arg):
    """The function executing the logic of instruction POPS.
    Load the symbol (cosntant or variable (its value and type)) from the data stack to given variable

    Arguments:
        arg(XMLElement): the variable or constant to stored value obtained from the stack
    """

    global data_stack, GF, LF, TF
    if not data_stack:
        print_err(56)
    check_exist_frame(arg.text.split('@')[0])
    check_exist_var(arg.text.split('@')[0], arg.text.split('@')[1])
    stack_elem = data_stack.pop()
    eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0] = stack_elem[0]
    eval(arg.text.split('@')[0])[arg.text.split('@')[1]][1] = stack_elem[1]

def check_3_arg(arg2, arg3, arg1=None):
    """The function executing the control of correctness the type of variables.
    Frame on which the variable is given must exists, variable on this frame mus
    exist and this variable must be defined. In other case error state is called.

    Arguments:
        arg2(XMLElement): second argument of instruction for this control
        arg3(XMLElement): third argument of instruction for this control
        arg1(XMLElement): check target variable, to which will be stored the result of instruction
    Return:
        string: array with name of frame and name of the variable such as target
    """

    global LF, TF, GF
    if arg1 is not None:
        check_exist_frame(arg1.text.split('@')[0])
        check_exist_var(arg1.text.split('@')[0], arg1.text.split('@')[1])
    if arg2.attrib['type'] in ('var'):
        check_exist_frame(arg2.text.split('@')[0])
        check_exist_var(arg2.text.split('@')[0], arg2.text.split('@')[1])
        check_undef_var(arg2.text.split('@')[0], arg2.text.split('@')[1])
    if arg3.attrib['type'] in ('var'):
        check_exist_frame(arg3.text.split('@')[0]) 
        check_exist_var(arg3.text.split('@')[0], arg3.text.split('@')[1])
        check_undef_var(arg3.text.split('@')[0], arg3.text.split('@')[1])
    if arg1 is not None:
        return arg1.text.split('@')

def check_type(arg, type_):
    """The function executing the control of type of given argument
    If the type of constant or variable not match with given type, error state is called.

    Arguments:
        arg(XMLElement): the variable or constant to control its type
        type_(string): the requested type
    """

    global LF, TF, GF
    operand = None
    if arg.attrib['type'] in ('string', 'bool', 'int'):
        if not(arg.attrib['type'] in (type_)):
            print_err(52)
        return arg.text
    if arg.attrib['type'] in ('var'):
        if not(eval(arg.text.split('@')[0])[arg.text.split('@')[1]][1] in (type_)):
            print_err(53)
        return eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0]

def check_same_type(arg2, arg3, arg1=None):
    """The function executing the control of type of two arguments, when the arguments
    must be of the identical type. If the type of constant or variable not match with given 
    type or the arguments not have the same type, error state is called.

    Arguments:
        arg2(XMLElement): second argument of instruction for this control
        arg3(XMLElement): third argument of instruction for this control
        arg1(XMLElement): check target variable, to which will be stored the result of instruction
    Return:
        string: array with name of frame and name of the variable such as target
    """

    global LF, TF, GF
    first_operand = dict()
    second_operand = dict()
    if arg2.attrib['type'] in ('var'):
        first_operand[0] = eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][0]
        first_operand[1] = eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][1]
    elif arg2.attrib['type'] in ('string', 'bool', 'int'):
        first_operand = {0: arg2.text, 1: arg2.attrib['type']}
    if arg3.attrib['type'] in ('var'):
        second_operand[0] = eval(arg3.text.split('@')[0])[arg3.text.split('@')[1]][0]
        second_operand[1] = eval(arg3.text.split('@')[0])[arg3.text.split('@')[1]][1]
    elif arg3.attrib['type'] in ('string', 'bool', 'int'):
        second_operand = {0: arg3.text, 1: arg3.attrib['type']}
    if arg2.attrib['type'] in ('int', 'bool', 'string') and arg3.attrib['type'] in ('int', 'bool', 'string'):
        if first_operand[1] != second_operand[1]:
            print_err(52)
    elif first_operand[1] != second_operand[1]:
        print_err(53)
    if arg1 is not None:
        eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'bool'
    return first_operand, second_operand 
     

def arithmetic(arg1, arg2, arg3, inst_name):
    """The function executing the logic of instruction ADD, SUB, MUL, IDIV.
    According to instruction name is execution the relevant operation between the arguments.

    Arguments:
        arg2(XMLElement): second argument of instruction
        arg3(XMLElement): third argument of instruction 
        arg1(XMLElement): target variable, to which will be stored the result of instruction
        inst_name(string): name of the instruction for decoding the operation
    """

    global LF, TF, GF
    arg1_text = check_3_arg(arg2, arg3, arg1)
    first_operand = check_type(arg2, 'int')
    second_operand = check_type(arg3, 'int')
    if inst_name in ('add'):
        eval(arg1_text[0])[arg1_text[1]][0] = int(first_operand) + int(second_operand)
    elif inst_name in ('sub'):
        eval(arg1_text[0])[arg1_text[1]][0] = int(first_operand) - int(second_operand)
    elif inst_name in ('mul'):
        eval(arg1_text[0])[arg1_text[1]][0] = int(first_operand) * int(second_operand)
    elif inst_name in ('idiv'):
        try:
            eval(arg1_text[0])[arg1_text[1]][0] = int(first_operand) // int(second_operand)
        except ZeroDivisionError:
            print_err(57)
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'int'

def recode(operand, mode):
    """The function transform the boolean text element to relevant type for work
    with these types or for print these types.

    Arguments:
        operand(string): the operand (text) for transform to the request form
        mode(int): flag for the diffs type of recoding of the operand
    Return:
        int/bool: recode type of the operand
    """

    if mode == 0:
        if operand.lower() in ('false'):
            return 0
        elif operand.lower() in ('true'):
            return 1
    elif mode == 1:
        if operand.lower() in ('false'):
            return False
        elif operand.lower() in ('true'):
            return True

def code_type(first_operand, second_operand):
    """The function for conversion the operand type and its value
    According to the type is executing the conversion on the request type.

    Arguments:
        first_operand(list): first operand to the conversion
        second_operand(list): second operand to the conversion
    Return:
        list: Operands with conversed type and same the value
    """

    if first_operand[1] in ('int'):
        first_operand = int(first_operand[0])
        second_operand = int(second_operand[0])
    elif first_operand[1] in ('bool'):
        first_operand = recode(first_operand[0], 0)
        second_operand = recode(second_operand[0], 0)
    elif first_operand[1] in ('string'):
        first_operand = first_operand[0]
        second_operand = second_operand[0]
    return first_operand, second_operand

def compare(arg1, arg2, arg3, inst_name):
    """The function executing the logic of instruction EQ, GT, LT.
    According to instruction name is execution the relevant operation between the arguments.

    Arguments:
        arg2(XMLElement): second argument of instruction
        arg3(XMLElement): third argument of instruction 
        arg1(XMLElement): target variable, to which will be stored the result of instruction
        inst_name(string): name of the instruction for decoding the operation
    """

    global LF, TF, GF
    arg1_text = check_3_arg(arg2, arg3, arg1)
    first_operand, second_operand = check_same_type(arg2, arg3, arg1)
    first_operand, second_operand = code_type(first_operand, second_operand)
    if inst_name in ('lt'):
        eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = str(first_operand < second_operand).lower()
    elif inst_name in ('gt'):
        eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = str(first_operand > second_operand).lower()
    elif inst_name in ('eq'):
        eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = str(first_operand == second_operand).lower()


def logic(arg1, arg2, arg3, inst_name):
    """The function executing the logic of instruction AND, OR.
    According to instruction name is execution the relevant operation between the arguments.

    Arguments:
        arg2(XMLElement): second argument of instruction
        arg3(XMLElement): third argument of instruction 
        arg1(XMLElement): target variable, to which will be stored the result of instruction
        inst_name(string): name of the instruction for decoding the operation
    """

    global LF, TF, GF
    arg1_text = check_3_arg(arg2, arg3, arg1)
    first_operand = check_type(arg2, 'bool')
    second_operand = check_type(arg3, 'bool')
    first_operand = recode(first_operand[0], 1)
    second_operand = recode(second_operand[0], 1)
    if inst_name in ('and'):
        eval(arg1_text[0])[arg1_text[1]][0] = str(first_operand and second_operand).lower()
    elif inst_name in ('or'):
        eval(arg1_text[0])[arg1_text[1]][0] = str(first_operand or second_operand).lower()
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'bool'

def check_2_arg(arg1, arg2, type_):
    """The function executing the control of correctness the type of variables.
    Frame on which the variable is given must exists, variable on this frame mus
    exist and this variable must be defined. In other case error state is called.

    Arguments:
        arg2(XMLElement): second argument of instruction for this control
        arg3(XMLElement): third argument of instruction for this control
        type_(string): the requested type of the variables
    Return:
        string: value of the variable or constant to execute the operation
    """

    global LF, TF, GF
    check_exist_frame(arg1.text.split('@')[0])
    check_exist_var(arg1.text.split('@')[0], arg1.text.split('@')[1])
    if arg2.attrib['type'] in (type_):
        return arg2.text
    elif arg2.attrib['type'] in ('var'):
        check_exist_frame(arg2.text.split('@')[0])
        check_exist_var(arg2.text.split('@')[0], arg2.text.split('@')[1])
        check_undef_var(arg2.text.split('@')[0], arg2.text.split('@')[1])
        if eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][1] in (type_):
            return eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][0]
        else:
            print_err(53)
    else:
        print_err(52)

def why_not(arg1, arg2):
    """The function executing the logic of instruction NOT.

    Arguments:
        arg2(XMLElement): argument of instruction for negation
        arg1(XMLElement): target variable, to which will be stored the result of instruction
    """

    global LF, TF, GF
    arg2.text = check_2_arg(arg1, arg2, 'bool')
    arg2.text = recode(arg2.text, 1)
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = str(not(arg2.text)).lower()
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'bool'

def int2char(arg1, arg2):
    """The function executing the logic of instruction INT2CHAR

    Arguments:
        arg2(XMLElement): argument of instruction for transformation to the char form
        arg1(XMLElement): target variable, to which will be stored the result of instruction
    """

    global LF, TF, GF
    arg2.text = check_2_arg(arg1, arg2, 'int')
    try:
        eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = back_transform(chr(int(arg2.text)))
    except ValueError:
        print_err(58)
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'string'

def stri2int(arg1, arg2, arg3):
    """The function executing the logic of instruction STRI2INIT

    Arguments:
        arg2(XMLElement): second argument of instruction -> index to target string, which will be replace
        arg3(XMLElement): third argument of instruction -> first char of this string such as sample for change 
        arg1(XMLElement): target variable, to which will be stored the result of instruction
    """

    global LF, TF, GF
    arg1_text = check_3_arg(arg2, arg3, arg1)
    first_operand = check_type(arg2, 'string')
    first_operand = transform_string(first_operand) #
    second_operand =  int(check_type(arg3, 'int'))
    if second_operand >= len(first_operand):
        print_err(58)
    try:
        eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = ord(first_operand[second_operand])
    except TypeError:
        print_err(58)
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'int'

def read(arg1, arg2):
    """The function executing the logic of instruction READ.

    Arguments:
        arg2(XMLElement): type for loading the values from stdin
        arg1(XMLElement): target variable, to which will be stored the loading value about the relevant type
    """

    global LF, TF, GF
    check_exist_frame(arg1.text.split('@')[0])
    check_exist_var(arg1.text.split('@')[0], arg1.text.split('@')[1])
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = arg2.text
    if arg2.text in ('int'):
        try:
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = int(input())
        except ValueError:
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = 0
    elif arg2.text in ('bool'):
        if input().lower() in ('true'):
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = 'true'
        else:
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = 'false'
    elif arg2.text in ('string'):
        try:
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = back_transform(input())
        except:
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = ''

def write(arg):
    """The function executing the logic of instruction WRITE.

    Arguments:
        arg(XMLElement): variable or constant to print to stdout
    """

    global LF, TF, GF
    tmp_str = 'ERROR'
    if arg.attrib['type'] in ('bool'):
        tmp_str = recode(arg.text, 1)
    elif arg.attrib['type'] in ('string'):
        tmp_str = transform_string(arg.text)
    elif arg.attrib['type'] in ('int'):
        tmp_str = int(arg.text)
    elif arg.attrib['type'] in ('var'):
        check_exist_frame(arg.text.split('@')[0])
        check_exist_var(arg.text.split('@')[0], arg.text.split('@')[1])
        check_undef_var(arg.text.split('@')[0], arg.text.split('@')[1])
        if eval(arg.text.split('@')[0])[arg.text.split('@')[1]][1] in ('string'):
            eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0] = transform_string(eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0])
        if eval(arg.text.split('@')[0])[arg.text.split('@')[1]][1] in ('int'):
            eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0] = int(eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0])
        tmp_str = eval(arg.text.split('@')[0])[arg.text.split('@')[1]][0]
    print(tmp_str)


def concat(arg1, arg2, arg3):
    """The function executing the logic of instruction CONCAT

    Arguments:
        arg2(XMLElement): second argument of instruction -> first string to concat
        arg3(XMLElement): third argument of instruction -> second string to concat
        arg1(XMLElement): target variable, to which will be stored the results string
    """

    global LF, TF, GF
    arg1_text = check_3_arg(arg2, arg3, arg1)
    first_operand = check_type(arg2, 'string')
    second_operand = check_type(arg3, 'string')
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = first_operand + second_operand
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'string'

def strlen(arg1, arg2):
    """The function executing the logic of instruction STRLEN

    Arguments:
        arg2(XMLElement): second argument of instruction -> string to measured the lenght
        arg1(XMLElement): target variable, to which will be stored the results length of given string
    """

    global LF, TF, GF
    tmp_str = check_2_arg(arg1, arg2, 'string')
    tmp_str = transform_string(tmp_str)
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = int(len(tmp_str))
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'int'

def getchar(arg1, arg2, arg3):
    """The function executing the logic of instruction GETCHAR

    Arguments:
        arg2(XMLElement): second argument of instruction 
        arg3(XMLElement): third argument of instruction
        arg1(XMLElement): target variable, to which will be stored the result
    """

    global LF, TF, GF
    arg1_text = check_3_arg(arg2, arg3, arg1)
    first_operand = check_type(arg2, 'string')
    first_operand =  transform_string(first_operand)
    second_operand =  int(check_type(arg3, 'int'))
    if second_operand >= len(first_operand):
        print_err(58)
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = back_transform(str(first_operand[second_operand]))
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'string'

def setchar(arg1, arg2, arg3):
    """The function executing the logic of instruction SETCHAR

    Arguments:
        arg2(XMLElement): second argument of instruction 
        arg3(XMLElement): third argument of instruction 
        arg1(XMLElement): target variable, to which will be stored the result
    """

    global LF, TF, GF
    arg1_text = check_3_arg(arg2, arg3, arg1)
    target = check_type(arg1, 'string')
    target = transform_string(target)
    first_operand = int(check_type(arg2, 'int'))
    second_operand = check_type(arg3, 'string')
    second_operand = transform_string(second_operand)
    if first_operand >= len(target) or second_operand in (''):
        print_err(58)
    if first_operand < 0:
        first_operand = len(target) + first_operand
        if first_operand < 0:
            print_err(58)
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = back_transform(target[:first_operand] + second_operand[0] + target[first_operand+1:])
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'string'

def type_(arg1, arg2):
    """The function executing the logic of instruction TYPE_

    Arguments:
        arg2(XMLElement): second argument of instruction 
        arg1(XMLElement): target variable, to which will be stored the result of instruction
    """

    global TF, LF, GF
    check_exist_frame(arg1.text.split('@')[0])
    check_exist_var(arg1.text.split('@')[0], arg1.text.split('@')[1])
    if arg2.attrib['type'] in ('int', 'bool', 'string'):
         eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = arg2.attrib['type']
    elif arg2.attrib['type'] in ('var'):
        check_exist_frame(arg2.text.split('@')[0])
        check_exist_var(arg2.text.split('@')[0], arg2.text.split('@')[1])
        if eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][0] is None:
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = ''
        else:
            eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][0] = eval(arg2.text.split('@')[0])[arg2.text.split('@')[1]][1]
    eval(arg1.text.split('@')[0])[arg1.text.split('@')[1]][1] = 'string'

def label(arg):
    """The function executing the logic of instruction LABEL

    Arguments:
        arg(XMLElement): the label element
    """

    pass

def jumps(inst_name, arg1, arg2, arg3, inst_reader, order_array):
    """The function executing the logic of instruction JUMPIFEQ, JUMPIFNEQ.

    Arguments:
        arg2(XMLElement): second argument of instruction -> to comparison
        arg3(XMLElement): third argument of instruction  -> to comparison
        arg1(XMLElement): label for the jump
        inst_reade(int): index to array with instruction order
        order_array(list): list with ordered instruction
    Return:
        inst_reader: index to list with instruction or the next executing instruction
    """

    global TF, LF, GF
    check_3_arg(arg2, arg3)
    first_operand, second_operand = check_same_type(arg2, arg3)
    first_operand, second_operand = code_type(first_operand, second_operand)
    if inst_name in ('jumpifeq') and first_operand == second_operand:
        return call(arg1, inst_reader, order_array, True)
    elif inst_name in ('jumpifneq') and first_operand != second_operand:
        return call(arg1, inst_reader, order_array, True)
    else:
        return inst_reader


""" EXTENSTIONS FOR SET OF THE STACK INSTRUCTIONS """

def get_operands(type_):
    global data_stack
    if not data_stack:
        print_err(56)
    second_operand = data_stack.pop()
    if not data_stack:
        print_err(56)
    first_operand = data_stack.pop()
    if type_ is not None:
        if first_operand[1] not in (type_) or second_operand[1] not in (type_):
            print_err(53)
    else:
        if first_operand[1] != second_operand[1]:
            print_err(53)
    return first_operand, second_operand
        

def arithmetic_S(inst_name):
    global data_stack
    first_operand, second_operand = get_operands('int')
    if inst_name in ('adds'):
        data_stack.append([int(first_operand[0]) + int(second_operand[0]), 'int'])
    elif inst_name in ('subs'):
        data_stack.append([int(first_operand[0]) - int(second_operand[0]), 'int'])
    elif inst_name in ('muls'):
        data_stack.append([int(first_operand[0]) * int(second_operand[0]), 'int'])
    elif inst_name in ('idivs'):
        try:
            data_stack.append([int(first_operand[0]) // int(second_operand[0]), 'int'])
        except ZeroDivisionError:
            print_err(57)

def compare_S(inst_name):
    global data_stack
    first_operand, second_operand = get_operands(None)
    first_operand, second_operand = code_type(first_operand, second_operand)
    if inst_name in ('lts'):
        data_stack.append([str(first_operand < second_operand).lower(), 'bool'])
    elif inst_name in ('gts'):
       data_stack.append([str(first_operand > second_operand).lower(), 'bool'])
    elif inst_name in ('eqs'):
       data_stack.append([str(first_operand == second_operand).lower(), 'bool'])

def logic_S(inst_name):
    global data_stack
    first_operand, second_operand = get_operands('bool')
    first_operand = recode(first_operand[0], 1)
    second_operand = recode(second_operand[0], 1)
    if inst_name in ('ands'):
        data_stack.append([str(first_operand and second_operand).lower(), 'bool'])
    elif inst_name in ('ors'):
        data_stack.append([str(first_operand or second_operand).lower(), 'bool'])

def get_operand(type_):
    global data_stack
    if not data_stack:
        print_err(56)
    operand = data_stack.pop()
    if operand[1] not in (type_):
        print_err(53)      
    return operand  

def not_S():
    global data_stack
    operand = get_operand('bool')
    operand[0] = recode(operand[0], 1)
    data_stack.append([str(not(operand[0])).lower(), 'bool'])

def int2char_S():
    global data_stack
    operand = get_operand('int')
    try:
        data_stack.append([back_transform(chr(int(operand[0]))), 'string'])
    except ValueError:
        print_err(58)

def stri2int_S():
    global data_stack
    second_operand = get_operand('int')
    second_operand = int(second_operand[0])
    first_operand  = get_operand('string')
    first_operand = transform_string(first_operand[0])
    if second_operand >= len(first_operand):
        print_err(58)
    try:
        data_stack.append([ord(first_operand[second_operand]), 'int'])
    except TypeError:
        print_err(58)

def jumps_S(inst_name, arg, inst_reader, order_array):
    global data_stack
    first_operand, second_operand = get_operands(None)
    first_operand, second_operand = code_type(first_operand, second_operand)
    if inst_name in ('jumpifeqs') and first_operand == second_operand:
        return call(arg, inst_reader, order_array, True)
    elif inst_name in ('jumpifneqs') and first_operand != second_operand:
        return call(arg, inst_reader, order_array, True)
    else:
        return inst_reader

""" DEBUG FUNCTIONS """
def break_():
    global LF, TF, GF, data_stack, call_stack, labels_dict
    print('\n-----------------------------', file=sys.stderr)
    print('Temporary frame: ', TF, file=sys.stderr)
    print('Global frame   : ', GF, file=sys.stderr)
    print('Local frame    : ', LF, file=sys.stderr)
    print('Data stack     : ', data_stack, file=sys.stderr)
    print('Call stack     : ', call_stack, file=sys.stderr)
    print('Labels dict    : ', labels_dict, file=sys.stderr)
    print('-----------------------------', file=sys.stderr)

def dprint(var):
    global LF, TF, GF
    if var is None:
        print('\nVariable is not defined!', file=sys.stderr)
    else:
        print('-----------------------------', file=sys.stderr)
        print('Variable', var.text, 'has:', file=sys.stderr)
        print('  value:', eval(var.text.split('@')[0])[var.text.split('@')[1]][0], file=sys.stderr)
        print('  type :',  eval(var.text.split('@')[0])[var.text.split('@')[1]][1], file=sys.stderr)
        print('-----------------------------', file=sys.stderr)

""" MAIN FUNCTION """       
def main(argv):
    global insts, vars
    help_flag = False
    insts_flag = False
    stats_flag = False
    vars_flag = False
    source_flag = False
    order = 0
    stats_namefile = ''
    xml_namefile = ''
    try:
        opts, _ = getopt.getopt(
            argv, "hs:s:iv", ['help', 'source=', 'stats=', 'insts', 'vars'])
    except getopt.GetoptError as err:
        print(str(err))
        sys.exit(99)
    for opt, arg in opts:
        if opt in ('-h','--help'):
            if stats_flag or insts_flag or vars_flag or source_flag:
                print_err(10)
            help_flag = True
        elif opt in ('-s', '--source'):
            if help_flag:
                print_err(10)
            source_flag = True
            xml_namefile = arg
        elif opt in ('-e', '--stats'):
            if help_flag or stats_flag:
                print_err(10)
            stats_namefile = arg
            stats_flag = True
        elif opt in ('-i', '--insts'):
            if help_flag or insts_flag:
                print_err(10)
            insts_flag = True
            if order == 0:
                order = 1
        elif opt in ('-v', '--vars'):
            if help_flag or vars_flag:
                print_err(10)
            vars_flag = True
            if order == 0:
                order = 2
    
    if help_flag:
        print_help()
    elif not os.path.exists(xml_namefile):
        print_err(11)
    else:
        parse_xml_file(xml_namefile)

    if (vars_flag or insts_flag) and not stats_flag:
        print_err(10)
    elif stats_flag and not os.path.exists(stats_namefile):
        print_err(12)
    
    vars = str(vars)
    insts = str(insts)
    if stats_flag and stats_namefile:
        with open(stats_namefile, 'w') as f:
            if vars_flag and insts_flag:
                if order == 1:
                    f.write(insts+ '\n' + vars + '\n')
                elif order == 2:
                    f.write(vars + '\n' + insts + '\n')
            elif vars_flag:
                f.write(vars + '\n')
            elif insts_flag:
                f.write(insts + '\n')

if __name__ == "__main__":
    main(sys.argv[1:])