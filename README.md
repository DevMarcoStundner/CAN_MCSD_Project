# CAN_MCSD_Project

The idea of the project is to devise your own individual embedded project using the evaluation board and one or a few sensors and actuators

<--[Diagram Image Link](//www.plantuml.com/plantuml/png/PP0n2uCm58Jt_8fNzmfc52bQwT315gnE8YAQqb9DIen3AVtlVGdDyEWKxlZktf5qCRqswponZRc9MK0dbHGTk-eUHR1NlHuX2k3Dcb8X-eA37DGeTCkQIrL0X6-UdI2VxiF3gW-DSXjr92UaLOKoIjj4Koz2mr4-LzF2TWV_fwY1aANyVVUw89r252HX61A2jn03IaQzn5VpNxNpqFHxlMdRtF3pQjpo6Egih5Wgv8WV-WK0)-->

<img src="./Component Diagram.JPG">

# CAN Interface

Das kann Interface wurde so aufgebaut, dass man damit ein Package bestehend aus folgenden Instanzen versenden kann.

> uint32_t id
> 
> uint8_t *data
> 
> uint8_t len

In der Library wurden insgesamt fünf Error-Codes definiert die über eine Funktion ausgelesen werden können.
> CAN_ERROR_NONE 
> 
> CAN_ERROR_EPV 
> 
> CAN_ERROR_BOF 
> 
> CAN_ERROR_STF 
> 
> CAN_ERROR_FOR 

Das Interface ist so aufgebaut, dass man mit Hilfe von Callbackfunktionen die verschiedenen Zustände verarbeiten kann.

Für die bessere Handhabung der Callbacks ist in der Library eine Funktion vorgesehen die die Functionpointer mit der richtigen ID in eine Array speichert.

Weiters kann neben dem typschen Versenden und Empfangen auch abgefragt werden welche Mailbox gerade frei ist.
