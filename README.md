# Event-Driven-Simulation-solution
 In this assignment, you will simulate customers arriving at a bank and standing in line in front of one of the tellers. People arrive at random intervals. Each person waits in his/her selected line until reaching  the head of that line. When a person reaches the head of his line, the teller provides service for a random  amount of time.


Name - Pratyush
Roll - 24155939

Name - Kartik Vibhu
Roll - 24155474


Summary -->

Here’s a **README-style summary** of your uploaded file (*Assignment 1: Event Driven Simulation*):

---


## Introduction

This project implements an **event-driven simulation** of customers arriving at a bank and waiting in line for tellers. The simulation models how customers arrive, queue, receive service, and leave, while collecting statistics on wait times and teller utilization.

The goal is to compare two queuing systems:

1. **Single Queue** – one common line serving all tellers.
2. **Multiple Queues** – each teller has a separate line.

---

## Assignment Description

* Customers arrive at random intervals.
* Each customer waits in line until served.
* Service times are random and based on an average service time parameter.
* Tellers may go idle if no customers are waiting.
* Simulation measures:

  * Average time spent by customers in the bank.
  * Standard deviation of wait times.
  * Maximum wait time.
  * Teller service time vs. idle time.

---

## Implementation

### Key Features

* **Event Queue:** Implemented as a linked list, sorted by event time.
* **Events:** Represent either customer arrivals, service completions, or teller availability.
* **Structures:**

  * `Customer` – arrival and service times.
  * `Teller` – service and idle states.
  * `Event` – contains action methods (via function pointers).
* **Queues:**

  * For tellers, each line is represented as a linked list (`tellerQueue`).
  * Customers are added to the end, removed from the front.

Actions

Customer Arrival:** Added to the shortest queue (or common queue).
Customer Service Completion:** Statistics collected; customer leaves.
Teller Idle:** Teller re-enters the event queue after a random idle time.

---

Input Format

The program is executed as:

```bash
./qSim #customers #tellers simulationTime averageServiceTime
```

Example:

```bash
./qSim 100 4 60 2.3
```

* `100` customers
* `4` tellers
* `60` minutes of simulation
* Average service time `2.3` minutes


Random Number Generation

Arrival Time:

  ```c
  float arrTime = simulationTime * rand() / float(RAND_MAX);
  ```
  Service Time:

  ```c
  float serviceTime = 2 * averageServiceTime * rand() / float(RAND_MAX);
  ```

Output

After running both queue types, the simulation prints:

* Total customers served and total time.
* Number of tellers and queuing system.
* Average time in bank + standard deviation.
* Maximum wait time.
* Teller service time and idle time.

A **GNUPlot graph** should be created showing average time vs. number of tellers (single queue).


 Deliverables

* **Code Files:** in `src`, `include`, `bin`, `output`.
* **Makefile:** target name `qSim`.
* **Test Cases:** at least three, showing outputs for both queue systems.
* **README.pdf:** Summary of program, run instructions, issues faced.
* **Analysis:** When single queue is better/worse than multiple queues.
