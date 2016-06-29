const net = require('net');

var server = net.createServer()
  .on('listening', () => {
      console.log("Listening on %s:%s", server.address().address, server.address().port);
    })
  .on('connection', (client) => {
    var client_addr = client.remoteAddress + ':' + client.remotePort;
    console.log("Connect: %s", client_addr);

    client
      .on('data', (buffer) => {
          data = buffer.toString();
          console.log("[%s] %s", client_addr,
                      data.endsWith('\n') ? data.slice(0, -1) : data);
          client.write(data);
        })
      .on('end', () => {
          console.log("Disconnect: %s", client_addr);
        })
      .on('error', (err) => {
          console.log('Error: %s: %s', client_addr, err.message);
        })
      })
  .listen(8080);
