require 'cocaine'



EventMachine.run do
  debugger = Cocaine::Service.new('debugger')
  deferred = debugger.connect
  puts "connecting..."
  deferred.callback do
    puts "connected"
    debugger.start('service')
    EventMachine.stop
  end
  deferred.errback do
    EventMachine.stop
  end
end
