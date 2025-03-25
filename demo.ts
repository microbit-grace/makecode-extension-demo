
/**
* Use this file to define custom functions and blocks.
* Read more at https://makecode.microbit.org/blocks/custom
* Play around at https://makecode.com/playground
*/

enum MyEnum {
    //% block="one"
    One,
    //% block="two"
    Two
}

/**
 * demo blocks
 */
//% weight=100 color=#0fbc11 icon=""
namespace demo {
    /**
     * TODO: describe your function here
     * @param n describe parameter here, eg: 5
     * @param s describe parameter here, eg: "Hello"
     * @param e describe parameter here
     */
    //% block
    export function foo(n: number, s: string, e: MyEnum): void {
        // Add code here
    }

    /**
     * TODO: describe your function here
     * @param value describe value here, eg: 5
     */
    //% block
    export function fib(value: number): number {
        return value <= 1 ? value : fib(value -1) + fib(value - 2);
    }

    //% shim=demo::startUartService
    //% block="someCppFunction"
    export function cppFunction(): void {
        return;
    }
}
