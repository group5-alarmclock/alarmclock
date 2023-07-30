import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:hive_flutter/adapters.dart';
import 'package:numberpicker/numberpicker.dart';

class TimerPage extends StatefulWidget {
  @override
  State<TimerPage> createState() => _TimePageState();
}

class _TimePageState extends State<TimerPage> {
  late TimeOfDay selectedTime1;
  late TimeOfDay selectedTime2;
  late String reps;

  bool changed = false;
  final myBox = Hive.box('mybox');
  TimerDataBase db = TimerDataBase();

  @override
  void initState() {
    if (myBox.get("STUDY") == null) {
      db.createInitialData();
    } else {
      db.loadData();
    }
    selectedTime1 = TimeOfDay(
        hour: int.parse(db.study.substring(0, 2)),
        minute: int.parse(db.study.substring(3, 5)));
    selectedTime2 = TimeOfDay(
        hour: int.parse(db.rest.substring(0, 2)),
        minute: int.parse(db.rest.substring(3, 4)));
    reps = db.reps;
    super.initState();
  }

  final dataBase = FirebaseDatabase.instance.reference();

  @override
  Widget build(BuildContext context) {
    Color iconColor = Color(0xFF008B8f);
    return Scaffold(
      backgroundColor: Colors.grey[300],
      appBar: AppBar(
        backgroundColor: Colors.grey[300],
        elevation: 0,
        title: const Text(
          'Study Mode',
          style: TextStyle(color: Color(0xFF008B8f), fontSize: 22),
        ),
        actions: [
          IconButton(
            icon: Icon(Icons.check,
                color: changed ? Color(0xFF008B8f) : Colors.grey[300], size: 30),
            onPressed: changed ?() {
              setState(() {
                changed = false;
                db.study = selectedTime1.toString().substring(10, 15);
                db.rest = selectedTime2.toString().substring(10, 15);
                db.reps = reps;
                db.updateDataBase();

                final child = dataBase.child('Study Mode');
                child.update({'study': (int.parse(db.study.substring(0,2))*100 + int.parse(db.study.substring(3,5))).toString()});
                child.update({'rest': (int.parse(db.rest.substring(0,2))*100 + int.parse(db.rest.substring(3,5))).toString()});
                child.update({'reps': db.reps});

                print("study = ${db.study.substring(0,2)}:${db.study.substring(3,5)}");
                print("rest = ${db.rest.substring(0,2)}:${db.rest.substring(3,5)}");
                print("reps = ${db.reps}");
              });
            } : null,
          ),
        ],
      ),
      body: Padding(
        padding: const EdgeInsets.symmetric(horizontal: 20),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.start,
          children: [
            const SizedBox(
              height: 20,
            ),
            Container(
              padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 10),
              decoration: BoxDecoration(
                color: Colors.grey[300],
                borderRadius: BorderRadius.circular(10),
              ),
              child: Column(
                children: [
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                    children: const [
                      Text(
                        'Study minutes',
                        style: TextStyle(fontSize: 20, color: Colors.black),
                      ),
                      Text(
                        '      Study seconds',
                        style: TextStyle(fontSize: 20, color: Colors.black),
                      ),
                    ],
                  ),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                    children: [
                      NumberPicker(
                        minValue: 0,
                        maxValue: 59,
                        value: selectedTime1.hour,
                        zeroPad: true,
                        infiniteLoop: true,
                        itemWidth: 80,
                        itemHeight: 70,
                        onChanged: (value) {
                          setState(() {
                            changed = true;
                            selectedTime1 =
                                selectedTime1.replacing(hour: value);
                          });
                        },
                        textStyle:
                            const TextStyle(color: Colors.grey, fontSize: 25),
                        selectedTextStyle: const TextStyle(
                            color: Color(0xFF008B8f), fontSize: 40),
                      ),
                      const Text(':',
                          style: TextStyle(
                              color: Color(0xFF008B8f), fontSize: 45)),
                      NumberPicker(
                        minValue: 0,
                        maxValue: 59,
                        value: selectedTime1.minute,
                        zeroPad: true,
                        infiniteLoop: true,
                        itemWidth: 80,
                        itemHeight: 70,
                        onChanged: (value) {
                          setState(() {
                            changed = true;
                            selectedTime1 =
                                selectedTime1.replacing(minute: value);
                          });
                        },
                        textStyle:
                            const TextStyle(color: Colors.grey, fontSize: 25),
                        selectedTextStyle: const TextStyle(
                            color: Color(0xFF008B8f), fontSize: 40),
                      ),
                    ],
                  ),
                  SizedBox(
                      width: MediaQuery.of(context).size.width * 0.92,
                      child: const Divider(
                          color: Colors.grey, thickness: 1.5, height: 25)),
                  const SizedBox(height: 20),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                    children: const [
                      Text(
                        'Rest minutes   ',
                        style: TextStyle(fontSize: 20, color: Colors.black),
                      ),
                      Text(
                        '      Rest seconds',
                        style: TextStyle(fontSize: 20, color: Colors.black),
                      ),
                    ],
                  ),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                    children: [
                      NumberPicker(
                        minValue: 0,
                        maxValue: 59,
                        value: selectedTime2.hour,
                        zeroPad: true,
                        infiniteLoop: true,
                        itemWidth: 80,
                        itemHeight: 70,
                        onChanged: (value) {
                          setState(() {
                            changed = true;
                            selectedTime2 =
                                selectedTime2.replacing(hour: value);
                          });
                        },
                        textStyle:
                            const TextStyle(color: Colors.grey, fontSize: 25),
                        selectedTextStyle: const TextStyle(
                            color: Color(0xFF008B8f), fontSize: 40),
                      ),
                      const Text(':',
                          style: TextStyle(
                              color: Color(0xFF008B8f), fontSize: 45)),
                      NumberPicker(
                        minValue: 0,
                        maxValue: 59,
                        value: selectedTime2.minute,
                        zeroPad: true,
                        infiniteLoop: true,
                        itemWidth: 80,
                        itemHeight: 70,
                        onChanged: (value) {
                          setState(() {
                            changed = true;
                            selectedTime2 =
                                selectedTime2.replacing(minute: value);
                          });
                        },
                        textStyle:
                            const TextStyle(color: Colors.grey, fontSize: 25),
                        selectedTextStyle: const TextStyle(
                            color: Color(0xFF008B8f), fontSize: 40),
                      ),
                    ],
                  ),
                ],
              ),
            ),
            SizedBox(
                width: MediaQuery.of(context).size.width * 0.81,
                child: const Divider(
                    color: Colors.grey, thickness: 1.5, height: 25)),
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
              children: [
                const Text(
                  'Repetitions',
                  style: TextStyle(fontSize: 20, color: Colors.black),
                ),
                Align(
                  alignment: Alignment.topLeft,
                  child: DropdownButton<String>(
                    dropdownColor: Colors.white,
                    value: reps,
                    icon: const Icon(
                      Icons.arrow_drop_down,
                      color: Colors.black,
                      size: 25,
                    ),
                    onChanged: (String? newValue) {
                      setState(() {
                        changed = true;
                        reps = newValue!;
                      });
                    },
                    style: const TextStyle(fontSize: 21, color: Colors.black),
                    items: const [
                      DropdownMenuItem<String>(
                        value: "1",
                        child: Text("1"),
                      ),
                      DropdownMenuItem<String>(
                        value: "2",
                        child: Text("2"),
                      ),
                      DropdownMenuItem<String>(
                        value: "3",
                        child: Text("3"),
                      ),
                      DropdownMenuItem<String>(
                        value: "4",
                        child: Text("4"),
                      ),
                      DropdownMenuItem<String>(
                        value: "5",
                        child: Text("5"),
                      ),
                      DropdownMenuItem<String>(
                        value: "6",
                        child: Text("6"),
                      ),
                      DropdownMenuItem<String>(
                        value: "7",
                        child: Text("7"),
                      ),
                      DropdownMenuItem<String>(
                        value: "8",
                        child: Text("8"),
                      ),
                      DropdownMenuItem<String>(
                        value: "9",
                        child: Text("9"),
                      ),
                      DropdownMenuItem<String>(
                        value: "10",
                        child: Text("10"),
                      ),
                    ],
                  ),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}

class TimerDataBase {
  String study = "";
  String rest = "";
  String reps = "";

  void createInitialData() {
    study = "50:00";
    rest = "10:00";
    reps = "4";
  }

  final myBox = Hive.box('mybox');

  void loadData() {
    study = myBox.get("STUDY");
    rest = myBox.get("REST");
    reps = myBox.get("REPS");
  }

  void updateDataBase() {
    myBox.put("STUDY", study);
    myBox.put("REST", rest);
    myBox.put("REPS", reps);
  }
}
