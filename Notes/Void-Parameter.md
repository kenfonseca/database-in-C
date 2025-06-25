What's the difference between:
* function()
* function(void)

if they both are used symbolize an absence of parameters?

With function(), you are allowed to give it as many parameters as you like
* Ex: function(123, 'h', "hello")

With function(void), you are given an errro if you try to give the function a parameter
* Ex: function(23) - ERROR