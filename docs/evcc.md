# Setting Up HeidelBridge With evcc

> [!IMPORTANT]
> Please make sure that you set up your HeidelBridge **before** adding it to evcc. Evcc won't start up, if it can't find your HeidelBridge! 

Adding your HeidelBridge to evcc is very straight forward.
Start by defining a new charger:

```
chargers:
  - name: heidelberg_ec
    type: template
    template: daheimladen-mb
    host: 192.168.178.133 # IP address or hostname
    port: 502 # Port (optional) 
```

Next, add a loadpoint:

```
loadpoints:
  - title: Heidelberg EC
    charger: heidelberg_ec
    mode: off
    guardduration: 5m
```

Aaaaaand you're done!
Start evcc with your new configuration and the HeidelBridge should be there.