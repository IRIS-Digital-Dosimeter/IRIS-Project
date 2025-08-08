# simple class 
# why is self called inside of  the class
#       - self.function() is the same as Bag.function()
#       - but we use self instead of Bag when calling it inside of "itself"


class Bag:
    # this function is holding the important data
    def __init__(self):
        # This is the data stored in a list
        self.data = []
        # this is a constant we just have stored in this class for other ppl to use 
        self.SubSammy_Cost_dollars = 3

    # this is a function that the class allows other ppl to use 
    def add(self, x):
        # all it does is add info to the "bag"
        self.data.append(x)

    # another function 
    def addtwice(self, x):
        # it add the data twice to the "bag"
        self.add(x)
        self.add(x)


def main():
    bag_instance = Bag()

    # Add elements to the bag
    bag_instance.add(5)
    bag_instance.add("Hello")
    bag_instance.add([1, 2, 3])

    # Add an element twice
    bag_instance.addtwice(10)

    # Print the contents of the bag
    print("Bag data:", bag_instance.data)
    print(f'A sammy Costs: ${bag_instance.SubSammy_Cost_dollars}')

    

if __name__ == "__main__": 

    # create an instance of the class available to the program 
    bag_instance = Bag()
    bag2 = Bag()
    # Check the data 
    print("Bag before adding data", bag_instance.data)
    print("Bag before adding data", bag2.data)
    # Use a function from the program 
    bag_instance.add(5)
    # View the contents/things you added by calling .data 
    print("Bag data after:", bag_instance.data)
    print("Bag data after:", bag2.data)
    # Check a value saved in 'bag" 
    print(f'A sammy Costs: ${bag_instance.SubSammy_Cost_dollars}')

    # try running main to see what this function does: 
    #main()