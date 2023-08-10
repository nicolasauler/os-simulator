# Oh!C

**Oh\!**perating 

**C**ystems

**C**mulator

Oh!C is an operating systems simulator that allows a user to:

- Select scheduler mode: choose between a FIFO mode or Round Robin
- Select size of memory to load in bit map

# Usage
## Initiating program
- For Round Robin mode
`./main -q <quantum_size>`

- For FIFO mode
`./main`

## Inside program
- For help menu input anything except create or kill

- To create a process
`create -m <mem_size>`

- To kill a process
`kill <pid>`

- To kill program press F1

## Example of running program

![Simulator running](./sim_so.png) 
