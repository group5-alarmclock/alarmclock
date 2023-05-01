import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'slider.dart';

class MainScreen extends StatefulWidget {
  @override
  _MainScreenState createState() => _MainScreenState();
}

class _MainScreenState extends State<MainScreen> {

  final dataBase = FirebaseDatabase.instance.reference();


  String autoBrightness = 'on';
  double currentVolume = 10;
  double brightness = 2;

  @override
  Widget build(BuildContext context) {



    return Scaffold(
      appBar: AppBar(
        leading: Icon(Icons.home),
        title: Text('Alarm Clock'),
      ),
      body: Column(
        mainAxisAlignment: MainAxisAlignment.start,
        children: [
          Text(
            'Change volume',
            style: TextStyle(
                color: Colors.blueGrey,
                fontSize: 35,
                fontWeight: FontWeight.bold),
          ),
          SizedBox(
            height: 20,
          ),
          Column(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              Text(currentVolume.toInt().toString(), style: TextStyle(fontSize: 30)),
                  Slider(
                      min: 0,
                      max: 100,
                      value: currentVolume,
                      onChanged: (value) {
                        setState(() {
                          currentVolume = value;
                          final child = dataBase.child('Audio');
                          child.update({'volume' : currentVolume.toInt()});
                        });
                      }
                  ),
            ],
          ),
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text(
                'Auto brightness ',
                style: TextStyle(fontSize: 25, fontWeight: FontWeight.bold),
              ),
              TextButton(
                  style: ButtonStyle(
                      padding:
                      MaterialStateProperty.all<EdgeInsets>(EdgeInsets.all(10)),
                      foregroundColor: MaterialStateProperty.all<Color>(
                        autoBrightness == 'off' ? Colors.red : Colors.green,
                      ),
                      shape: MaterialStateProperty.all<RoundedRectangleBorder>(
                          RoundedRectangleBorder(
                              borderRadius: BorderRadius.circular(15.0),
                              side: BorderSide(
                                  color: autoBrightness == 'off' ? Colors.red : Colors.green)))),
                  onPressed: () {
                    autoBrightness = autoBrightness == 'off' ? 'on' : 'off';
                    final child = dataBase.child('Display');
                    child.update({'auto brightness' : autoBrightness == 'on' ? 'on' : 'off'});
                    setState(() {});
                  },
                  child: Text(
                    autoBrightness == 'off' ? 'off' : 'on',
                    style: TextStyle(fontSize: 25),
                  )),
              SizedBox(
                height: 20,
              ),
            ],
          ),
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text(
                autoBrightness == 'off' ? "Set brightness:" : "",
                style: TextStyle(
                    color: Colors.blueGrey,
                    fontSize: 25,
                    fontWeight: FontWeight.bold),
              ),
              SliderTheme(
                data: SliderThemeData(
                  disabledActiveTrackColor: Colors.blue,
                  disabledInactiveTrackColor: Colors.black12,
                  trackHeight: autoBrightness == 'on' ? 0 : 25,
                  thumbShape: RoundSliderThumbShape(enabledThumbRadius: 0.0),
                ),
                child: Slider(
                  min: 0,
                  max: 15,
                  value: brightness,
                  onChanged: (value) {
                    setState(() {
                      brightness = value;
                      final child = dataBase.child('Display');
                      child.update({'brightness' : brightness.toInt()});
                    });
                  },
                ),
              ),
            ],
          )
        ],
      ),
    );
  }
}