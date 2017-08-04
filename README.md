<snippet>
<content>

# Tableu Prover
An implementation of a tableau which tells you whether a formula of propositional logic is satisfiable or not. If it is satisfiable it will output one possible solution.

## Installation
* On the command line enter: `git clone https://github.com/Hamid1993/tableau-prover.git`
* Change to the directory of the program TableauProver `cd somepath/tableau-prover/TableauProver`
* provide an inputfile called `input.txt` or change the makefile to support your inputfile name 
* run the program by typing `make run`
* the result will be a file called `output.txt`

## Usage
The program expects at least one command line argument, a text file containing your formulas. The formulas must follow the following rules described by the Backus-Naur form in the table below:
     
     
    <formula> ::=   p
    <formula> ::=   -<formula>
    <formula> ::=   (<formula><op><formula>)
    <op>      ::=   v|^|>|<

where `p` is a single literal consisting of a single character. 
 `v` is the disjunction operator
`^` is the conjunction operator
 `>`right arrow implication
 `<`left array implication
 
 You need to start a new line for every single formula. It is not allowed to have whitespace within a formula. You may not use any `<op>` symbols as literal. 
Additionally you can provide a filename for your second argument,but then you cannot use the makefile provided here. You will need to edit the makefile or compile and run the program from the terminal manually. If you decide to provide an additional argument the program will name the output result file with the name given from you. Otherwise the outputfile will just be called `output.txt`
##Example
An example file called `input.txt` is provided in the repository. An input file could like like this


    -(p>p)
    -()
    -(p>(q>p))
    ((p>q)>p)
    --((pvq)>(-p^-q))
    (p^-p)
    ((p>q)^(q>p))
    -((p>(qvr))>((p>q)v(p>r)))


The `output.txt` would look like this:


    ---------Line 1-------------
    "-(p>p)" is not satisfiable
    ---------Line 2-------------
    "-()" not a formula.
    --------Line 3-------------
    "-(p>(q>p))" is not satisfiable
    ---------Line 4-------------
    "((p>q)>p)" is satisfiable. 
    p=1,q=0,
    ---------Line 5-------------
    "--((pvq)>(-p^-q))" is satisfiable. 
    p=0,q=0,
    ---------Line 6-------------
    "(p^-p)" is not satisfiable
    ---------Line 7-------------
    "((p>q)^(q>p))" is satisfiable. 
    p=0,q=0,
    ---------Line 8-------------
    "-((p>(qvr))>((p>q)v(p>r)))" is not satisfiable


## Contributing
1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request :D
## License
This project is licensed under the MIT License - see the LICENSE.md file for details

</content>
  <tabTrigger>readme</tabTrigger>
</snippet>
