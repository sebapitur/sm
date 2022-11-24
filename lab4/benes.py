from functools import reduce

def shuffle(input: int, size: int) -> int:
    if input % 2 == 0:
        return input // 2
    return 1 << (size - 1) | (input // 2)

def pad_left(stri: str, length: int) -> str:
    if length == 0:
        return stri
    if length == 1:
        return '0' + stri
    zeroes = ['0' for i in range(0, length)]
    padding = reduce(lambda a, b: a + b, zeroes)
    return padding + stri

def network(source :int, destination :int, power :int, floors :int):
    network_size = power
    source_str = list(reversed(list("{0:b}".format(source))))
    destination_str = list(reversed(list("{0:b}".format(destination))))
    source_str =  reduce(lambda a, b: a + b, source_str)
    destination_str =  reduce(lambda a, b: a + b, destination_str)
    source_str = pad_left(source_str, power - len(source_str))
    destination_str = pad_left(destination_str, power - len(destination_str))
    floor_start = [[],[]]

    output = str("")
    step = 0

    # first half
    while (step < floors // 2):
        output = output + 'd'
        source = shuffle(source, network_size)
        floor_start[0].append(source)

        if (source >= (1 << (power - step)) // 2 and
            step < power // 2 and floors > 3):
            source -= (1 << (power - step)) // 2
            floor_start[1].append(1)
        else:
            floor_start[1].append(0)

        step += 1
        network_size -= 1

    # middle
    if (source_str[0] == destination_str[0]):
        output += 'd'
    else:
        output += 'i'
        source ^= 1
    
    step += 1
    floor_start[0].append(source)
    floor_start[1].append(0)
    network_size = power

    # second half
    i = floors - 1
    second_half_output = str("")

    while i >= step + 1:
        if (destination & 1):
            if (floor_start[1][floors - i - 1]):
                second_half_output +=  'd'
                destination = shuffle(destination, network_size)
            else:
                second_half_output +=  'i'
                destination = shuffle(destination ^ 1, network_size)
            
        else:
            if (floor_start[1][floors - i - 1]):
                second_half_output +=  'i'
                destination = shuffle(destination ^ 1, network_size)
            else:
                second_half_output +=  'd'
                destination = shuffle(destination, network_size)  

        if destination >= (1 << power) // 2:
            destination -= (1 << power) // 2
        
        i -= 1
        network_size -= 1
    second_half_output = list(reversed(second_half_output))
    second_half_output = reduce(lambda a, b: a + b, second_half_output) 
    output += second_half_output

    # last ties
    if (destination == source):
        output +='d'
    else:
        if (destination > source):
            if (destination % 2 == 1):
                output +='i'
                destination = shuffle(destination ^ 1, network_size)
            else:
                output +='d'
                destination = shuffle(destination, network_size)
            
        else:
            if (destination % 2 == 1):
                output +='d'
                destination = shuffle(destination, network_size)
            else:
                output +='i'
                destination = shuffle(destination ^ 1, network_size)
    
    # display result
    for direction in output:
        if direction == 'd':
            print("direct", end=' ')
        else:
            print("invers", end=' ')


def main():

    k = int(input())
    n = 2**k
    source = []
    floors = 2 * k - 1

    for i in range(0, n):
        source.append(int(input()))

    destination = range(0, n)

    for i in range(0, n):
        print("Start point: {} and Finish point {}".format(source[i], destination[i]))
        network(source[i], destination[i], k, floors)
        print()


if __name__ == "__main__":
    main()
