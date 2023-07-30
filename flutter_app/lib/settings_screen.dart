import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:hive_flutter/adapters.dart';

class SettingsScreen extends StatefulWidget {
  @override
  _SettingsScreen createState() => _SettingsScreen();
}

class _SettingsScreen extends State<SettingsScreen> {
  final dataBase = FirebaseDatabase.instance.reference();
  final myBox = Hive.box('mybox');
  SettingsDataBase db = SettingsDataBase();
  String? selectedLanguage = "1";
  final LanguageList = ["", "english", "arabic", "hebrew", "russian", "chinese"];

  @override
  void initState() {
    if (myBox.get("SETTINGS") == null) {
      db.createInitialData();
    } else {
      db.loadData();
    }
    super.initState();
  }


  @override
  Widget build(BuildContext context) {
    bool switchValue = true;
    return Scaffold(
      backgroundColor: Colors.grey[300],
      appBar: AppBar(
        iconTheme: const IconThemeData(
          color: Color(0xFF008B8f), // Change this color to your desired color
        ),
        title: const Text(
          'Settings',
          style: TextStyle(
            color: Color(0xFF008B8f),
            fontSize: 22,
          ),
        ),
        elevation: 0,
        backgroundColor: Colors.grey[300],
      ),
      body: Column(
        mainAxisAlignment: MainAxisAlignment.start,
        children: [
          Padding(
            padding: const EdgeInsets.symmetric(vertical: 8, horizontal: 8),
            child: Container(
              padding: const EdgeInsets.all(10),
              decoration: BoxDecoration(
                borderRadius: BorderRadius.circular(25.0),
                color: Colors.white,
              ),
              child: Column(
                children: [
                  Container(
                      child: const Align(
                    alignment: Alignment.topLeft,
                    child: Text(
                      ' Volume',
                      textAlign: TextAlign.left,
                      style: TextStyle(
                        color: Colors.black,
                        fontSize: 22,
                      ),
                    ),
                  )),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      const Icon(Icons.volume_up_sharp,
                          size: 30, color: Colors.black), // Sound
                      SizedBox(
                        width: MediaQuery.of(context).size.width * 0.83,
                        child: Slider(
                            activeColor: Color(0xFF008B8f),
                            inactiveColor: Colors.blueGrey,
                            min: 0,
                            max: 21,
                            value: db.Settings[0].toDouble(),
                            onChanged: (value) {
                              setState(() {
                                db.Settings[0] = value;
                                final child = dataBase.child('Settings/Audio');
                                db.updateDataBase();
                                child.update({
                                  'volume': db.Settings[0].toInt().toString()
                                });
                              });
                            }),
                      )
                    ],
                  ),
                ],
              ),
            ),
          ),
          Padding(
            padding: const EdgeInsets.symmetric(vertical: 8, horizontal: 8),
            child: Container(
              padding: const EdgeInsets.all(10),
              decoration: BoxDecoration(
                borderRadius: BorderRadius.circular(25.0),
                color: Colors.white,
              ),
              child: Column(
                children: [
                  Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      const Align(
                        alignment: Alignment.topLeft,
                        child: Text(
                          " Brightness",
                          style: TextStyle(color: Colors.black, fontSize: 22),
                        ),
                      ),
                      Row(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          const Icon(Icons.brightness_6_outlined,
                              size: 30, color: Colors.black), // Sound
                          SizedBox(
                            width: MediaQuery.of(context).size.width * 0.83,
                            child: Slider(
                              activeColor: Color(0xFF008B8f),
                              inactiveColor: Colors.blueGrey,
                              min: 0,
                              max: 15,
                              value: db.Settings[1].toDouble(),
                              onChanged: (value) {
                                setState(() {
                                  db.Settings[1] = value;
                                  db.Settings[2] == 'on' ? 'off' : 'off';
                                  db.updateDataBase();
                                  final child =
                                      dataBase.child('Settings/Display');
                                  child.update({
                                    'brightness':
                                        db.Settings[1].toInt().toString()
                                  });
                                });
                              },
                            ),
                          ),
                        ],
                      ),
                    ],
                  ),
                  Container(
                    width: MediaQuery.of(context).size.width * 0.88,
                    child: const Divider(
                        color: Colors.grey, thickness: 1.5, height: 25),
                  ),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      const Text(
                        ' Auto brightness',
                        style: TextStyle(fontSize: 22, color: Colors.black),
                      ),
                      const Spacer(),
                      Transform.scale(
                          scale: 1.1,
                          child: Switch(
                              value: db.Settings[2] == 'on',
                              onChanged: (value) {
                                setState(() {
                                  db.Settings[2] = value ? 'on' : 'off';
                                  db.updateDataBase();
                                  final child =
                                      dataBase.child('Settings/Display');
                                  child.update({
                                    'auto brightness': db.Settings[2],
                                  });
                                });
                              },
                              activeColor: Colors.grey[100],
                              activeTrackColor:
                                  Color(0xFF008B8f).withOpacity(1),
                              inactiveTrackColor: Colors.grey.withOpacity(0.5),
                              materialTapTargetSize:
                                  MaterialTapTargetSize.shrinkWrap)),
                      const SizedBox(
                        height: 20,
                      ),
                    ],
                  ),
                ],
              ),
            ),
          ),
          Padding(
            padding: const EdgeInsets.symmetric(vertical: 8, horizontal: 8),
            child: Container(
              padding: const EdgeInsets.all(15),
              decoration: BoxDecoration(
                borderRadius: BorderRadius.circular(25.0),
                color: Colors.white,
              ),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [
                  const Text(
                    'Choose Language',
                    style: TextStyle(
                        fontSize: 22,
                        color: Colors.black,
                        fontWeight: FontWeight.w400),
                  ),
                  Align(
                    alignment: Alignment.topLeft,
                    child: DropdownButton<String>(
                      dropdownColor: Colors.white,
                      icon: const Icon(
                        Icons.arrow_drop_down,
                        color: Colors.black,
                        size: 25,
                      ),
                      onChanged: (String? newValue) {
                        setState(() {
                          selectedLanguage = newValue;
                          final child = dataBase.child('Settings');
                          child.update({
                            'Language': LanguageList[int.parse(selectedLanguage!)],
                          });
                        });
                      },
                      style: const TextStyle(fontSize: 21, color: Colors.black),
                      value: selectedLanguage, // Set the selected value
                      items: const [
                        DropdownMenuItem<String>(
                          value: "1",
                          child: Text("English"),
                        ),
                        DropdownMenuItem<String>(
                          value: "2",
                          child: Text("Arabic"),
                        ),
                        DropdownMenuItem<String>(
                          value: "3",
                          child: Text("Hebrew"),
                        ),
                        DropdownMenuItem<String>(
                          value: "4",
                          child: Text("Russian"),
                        ),
                        DropdownMenuItem<String>(
                          value: "5",
                          child: Text("Chinese"),
                        ),
                      ],
                    ),
                  ),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class SettingsDataBase {
  List Settings = [];

  void createInitialData() {
    // [volume, brightness, auto brightness, Language]
    Settings = [5, 5, "on", 1];
  }

  final myBox = Hive.box('mybox');

  void loadData() {
    Settings = myBox.get("SETTINGS");
  }

  void updateDataBase() {
    myBox.put("SETTINGS", Settings);
  }
}
