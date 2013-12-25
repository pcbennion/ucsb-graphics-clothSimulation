Peter Bennion
CS180: Computer Graphics
Project 6: Cloth Simulation

Submission is designed to be run on the computers at
UCSB's CSIL tech lab. Makefile and includes are for a
Linux Fedora distribution, with GLU and GLUT installed.

This is a forward euler simulation of cloth, falling from
a horizontal position. It uses damped springs to execute
the simulation stably.

The OpenGL in this program is very simplistic, with no
shadows or depth. In addition, significant improvements
can be made to the stability of the particles, especially
for large number of verticies.