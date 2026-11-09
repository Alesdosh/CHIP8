# Emulador de CHIP-8

Emulador del Chip-8. Construido en **C**.

## Descripción

Por la definición formal: 

> CHIP-8 es un lenguaje de programación interpretado, desarrollado por Joseph Weisbecker. Fue inicialmente usado en los microcomputadores de 8 bits COSMAC VIP y Telmac 1800 a mediados de 1970. Los programas de CHIP-8 corren sobre una máquina virtual de CHIP-8.

Este emulador de la maquina virtual Chip-8, es capaz de ejecutar archivos con la extensión: .**chip8**, desarrollado esencialmente en **C** junto con **Cmake** para los procesos de compilación y construcción de los ejecutables necesarios, y **SDL3** para el manejo de graficos, sonidos y mapeo del input mediante el teclado.

Utilizando como principal fuente de informacion y recursos, la guia: How to write an emulator (CHIP-8 interpreter) 
*https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/*

Ademas de la pagina de Wikipedia oficial CHIP-8
*https://es.wikipedia.org/wiki/CHIP-8*


## Arquitectura General

Para comenzar a desarrollar un emulador como CHIP-8, es necesario conocer los requisitos de que esta *"maquina virtual"* funcione y ejecute los archivos correspondientes correctamente.
Me base en la informacion que proporciona wikipedia y otras fuentes sobre lo que necesitaria para crear este emulador, que vendria siendo lo siguiente:

## MEMORIA

La memoria del chip-8 es de 8,548 bytes, teniendo un rango desde 200h a FFFh

## REGISTROS

Chip-8 cuenta con 16 registros de 8-bits, para datos, se suelen llamar V0, V1, V2, etc.

## PILA O STACK

La pila del chip-8 se usa para almacenar direcciones que seran usadas luego, al regresar de una subrutina

## TIMERS

El chip-8 cuenta con dos timers:

1. Sound Timer: Consiste en un temporizador usado para el sonido del juego, cuando el sonido no es 0, se escucha un beep

2. Delay Timer: Temporizador usado para sincronizar los eventos del juego

## INPUT

EL chip-8 cuenta con un teclado de 16 teclas en un rango de 0 a F

## GRAFICOS Y SONIDO

La resolucion estandar de pantalla es de 64 * 32 pixeles, solo cuenta con dos colores, osea que es monocromo.

Los sprites del chip-8 son de 8 pixeles de ancho por 1 a 15 de alto

## OPCODES

Los opcodes en el chip-8 son las instrucciones que es capaz de detectar y "ejecutar" dentro de el, cuenta con tan solo 35, usadas para distintos propositos cada una, desde solo cambiar el valor de un registro por ejemplo, hasta recibir el input del usuario y "mapearlo"

