# BLTickRate
## About
This is a DLL hack used to modify the base tick rate of Blockland. This DLL is not yet updated for Blockland r2033.

## Usage
This is a special DLL. It needs to run before engine classes are registered with the console. Use Stud_PE or a similar tool to add the DLL to the exe's import table, there is an export named "hello" you can select.

## Compiling
Simply point CMake to the root directory, configure, and build. There are no dependencies!

## Resources
As tick rate increases, so does the packet send rate. You will have to manually set the `$Pref::Net::PacketRateTo*` variables after the game scripts are done executing, since they clamp the values to 32 or under.

Here is a semi-related script that can maximize packet send rate without the DLL.

```
function NetConnection::setMaxPacketRate(%t)
{
    %c = $pref::Net::PacketRateToClient;
    %s = $pref::Net::PacketRateToServer;

    %l = $Server::LAN;
    %d = doesAllowConnections();

    $Server::LAN = 1;
    setAllowConnections(0);

    %t.checkMaxRate();

    setAllowConnections(%d);
    $Server::LAN = %l;
   
    $pref::Net::PacketRateToClient = %c;
    $pref::Net::PacketRateToServer = %s;
}

package SetMaxPacketRate
{
    function GameConnection::startLoad(%t)
    {
        %t.setMaxPacketRate();
   
        return Parent::startLoad(%t);
    }
};

activatePackage(SetMaxPacketRate);
```