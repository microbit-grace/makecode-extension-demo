basic.showIcon(IconNames.Surprised)
demo.cppStartUartService()
demo.cppStartUtilityService()
datalogger.setColumnTitles("apple")
datalogger.log(datalogger.createCV("apple", 1))
datalogger.log(datalogger.createCV("apple", 3))
basic.forever(function () {
    demo.cppLogReadData()
})
