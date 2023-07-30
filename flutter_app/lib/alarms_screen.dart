import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'pages/Alarm/timePage.dart';
import 'package:hive_flutter/hive_flutter.dart';

class AlarmsScreen extends StatefulWidget {
  const AlarmsScreen({Key? key}) : super(key: key);

  @override
  _AlarmsScreenState createState() => _AlarmsScreenState();
}

class _AlarmsScreenState extends State<AlarmsScreen> {
  final myBox = Hive.box('mybox');
  AlarmDataBase db = AlarmDataBase();

  @override
  void initState() {
    if (myBox.get("ALARM") == null) {
      db.createInitialData();
    } else {
      db.loadData();
    }
    super.initState();
  }

  final dataBase = FirebaseDatabase.instance.reference();
  List<Alarm> alarms = [];

  void _addAlarm() {
    Navigator.push<Alarm>(
      context,
      MaterialPageRoute(
          builder: (context) => TimePage(
                initialTime: TimeOfDay.fromDateTime(
                    DateTime.parse('2018-10-20 07:00:04Z')),
                day: 'NNNNNNN',
                name: '',
                ringtone: 1,
                level: 1,
                on: true,
                number: db.alarmAvail.indexOf(true),
              )),
    ).then((alarm) {
      if (alarm != null) {
        setState(() {
          final child =
              dataBase.child('Alarms/alarm${db.alarmAvail.indexOf(true) + 1}');
          child.update({'level': alarm.level.toString()});
          child.update({'mode': alarm.on == true ? "1" : "0"});
          child.update({'ringtone': alarm.sound.toString()});
          child.update({'time': alarm.time.toString().substring(10, 15)});
          child.update({'wdays': '[${alarm.days.replaceAll("N", "")}]'});

          // Save to Firebase
          alarms.add(Alarm(
            time: alarm.time,
            days: alarm.days,
            name: alarm.name,
            sound: alarm.sound,
            level: alarm.level,
            on: alarm.on,
            number: db.alarmAvail.indexOf(true),
          ));

          // Save to Database
          db.alarmList.add([
            alarm.time.toString().substring(10, 15),
            alarm.days,
            alarm.name,
            alarm.sound,
            alarm.level,
            alarm.on,
            db.alarmAvail.indexOf(true),
          ]);
          db.updateDataBase();
          db.alarmAvail[db.alarmAvail.indexOf(true)] = false;

          for (var entry in db.alarmList) {
            print(entry);
          }
        });
      }
    });
  }

  void _updateAlarm(Alarm initAlarm, int index) async {
    Alarm? alarm = await Navigator.push<Alarm>(
      context,
      MaterialPageRoute(
          builder: (context) => TimePage(
              initialTime: initAlarm.time,
              day: initAlarm.days,
              name: initAlarm.name,
              ringtone: initAlarm.sound,
              level: initAlarm.level,
              on: initAlarm.on,
              number: initAlarm.number)),
    );
    if (alarm != null) {
      setState(() {
        final child = dataBase.child('Alarms/alarm${alarm.number + 1}');
        child.update({'level': alarm.level.toString()});
        child.update({'mode': alarm.on == true ? "1" : "0"});
        child.update({'ringtone': alarm.sound.toString()});
        child.update({'time': alarm.time.toString().substring(10, 15)});
        child.update({'wdays': '[${alarm.days.replaceAll("N", "")}]'});

        // Save to Database
        db.alarmList[index][0] = alarm.time.toString().substring(10, 15);
        db.alarmList[index][1] = alarm.days;
        db.alarmList[index][2] = alarm.name;
        db.alarmList[index][3] = alarm.sound;
        db.alarmList[index][4] = alarm.level;
        db.alarmList[index][5] = alarm.on;
        db.alarmList[index][6] = alarm.number;
        db.updateDataBase();

        for (var entry in db.alarmList) {
          print(entry);
        }
      });
    }
  }

  Future<void> _deleteAlarm(Alarm initAlarm, int index) async {
    setState(() {
      final child = dataBase.child('Alarms');
      child.update({'alarm${initAlarm.number + 1}': null});
      db.alarmAvail[initAlarm.number] = true;
      // alarms.removeAt(index);

      db.alarmList.removeAt(index);
      db.updateDataBase();
    });
  }

  @override
  Widget build(BuildContext context) {
    final List color = [Colors.white, Colors.black];
    List<String> daysOfWeek = ['S', 'M', 'T', 'W', 'T', 'F', 'S'];
    List<String> Week = ['Sun', 'Mon', 'Tue', 'Wed', 'Thurs', 'Fri', 'Sat'];
    DateTime today = DateTime.now();
    TimeOfDay time = TimeOfDay.now();
    bool _switchValue = true;

    return Scaffold(
      backgroundColor: Colors.grey[300],
      appBar: AppBar(
        backgroundColor: Colors.grey[300],
        elevation: 0,
        title: const Text(
          'Alarm',
          style: TextStyle(color: Color(0xFF008B8f), fontSize: 22),
        ),
        actions: [
          IconButton(
            icon: const Icon(Icons.add, color: Color(0xFF008B8f), size: 33),
            onPressed: _addAlarm,
          ),
        ],
      ),
      body: SingleChildScrollView(
        child: Center(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              const SizedBox(height: 16),
              ListView.builder(
                itemCount: db.alarmList.length,
                shrinkWrap: true,
                physics: NeverScrollableScrollPhysics(),
                itemBuilder: (context, index) {
                  final alarm = Alarm(
                    time: TimeOfDay(
                        hour: int.parse(db.alarmList[index][0].substring(0, 2)),
                        minute:
                            int.parse(db.alarmList[index][0].substring(3, 5))),
                    days: db.alarmList[index][1],
                    name: db.alarmList[index][2],
                    sound: db.alarmList[index][3],
                    level: db.alarmList[index][4],
                    on: db.alarmList[index][5],
                    number: db.alarmList[index][6],
                  );
                  return GestureDetector(
                    onTap: () {
                      _updateAlarm(alarm, index);
                    },
                    child: Padding(
                      padding: const EdgeInsets.all(5.0),
                      child: Container(
                        decoration: BoxDecoration(
                          color: Colors.white,
                          borderRadius: BorderRadius.circular(20),
                        ),
                        width: MediaQuery.of(context).size.width * 0.96,
                        height: 135,
                        child: Column(
                          children: [
                            const SizedBox(height: 12),
                            Align(
                              alignment: Alignment.centerLeft,
                              child: Row(
                                children: [
                                  const Text('     '),
                                  Text(
                                    db.alarmList[index][2].isEmpty
                                        ? 'alarm ${index + 1}'
                                        : db.alarmList[index][2],
                                    style: const TextStyle(
                                        color: Colors.black, fontSize: 20),
                                  ),
                                ],
                              ),
                            ),
                            const SizedBox(height: 8),
                            Row(
                              children: [
                                const Text('     '),
                                Text(
                                  '${db.alarmList[index][0].substring(0, 2).padLeft(2, '0')}:${db.alarmList[index][0].substring(3, 5).padLeft(2, '0')}  ',
                                  style: const TextStyle(
                                      color: Colors.black, fontSize: 25),
                                ),
                                const Spacer(),
                                Transform.scale(
                                  scale: 1.05,
                                  child: Switch(
                                      value: db.alarmList[index][5],
                                      onChanged: (value) {
                                        setState(() {
                                          _switchValue = !value;
                                          db.alarmList[index][5] =
                                              !db.alarmList[index][5];
                                          final child = dataBase.child(
                                              'Alarms/alarm${db.alarmList[index][6] + 1}');
                                          child.update({
                                            'mode': db.alarmList[index][5]
                                                ? "1"
                                                : "0"
                                          });
                                        });
                                      },
                                      activeColor: Colors.grey[100],
                                      activeTrackColor:
                                          Color(0xFF008B8f),
                                      inactiveTrackColor:
                                          Colors.grey.withOpacity(0.5),
                                      materialTapTargetSize:
                                          MaterialTapTargetSize.shrinkWrap),
                                ),
                              ],
                            ),
                            Row(
                              children: [
                                const Text('     '),
                                if (db.alarmList[index][1]
                                        .split('')
                                        .where((c) => c == 'N')
                                        .length ==
                                    7)
                                  Text(
                                    int.parse(db.alarmList[index][0]
                                                    .toString()
                                                    .substring(0, 2)) <
                                                time.hour ||
                                            (int.parse(db.alarmList[index][0]
                                                        .toString()
                                                        .substring(0, 2)) ==
                                                    time.hour &&
                                                int.parse(db.alarmList[index][0]
                                                        .toString()
                                                        .substring(3, 5)) <
                                                    time.minute)
                                        ? 'Tomorrow  ${Week[(today.add(Duration(days: 1)).weekday) % 7]}, ${today.add(Duration(days: 1)).day}/${today.add(Duration(days: 1)).month}'
                                        : 'Today ${Week[(today.weekday) % 7]}, ${today.day}/${today.month}',
                                    style: const TextStyle(
                                        color: Colors.black, fontSize: 17,
                                        fontWeight: FontWeight.w500),
                                  )
                                else
                                  for (int i = 0; i < 7; i++)
                                    if (db.alarmList[index][1]
                                        .contains(i.toString()))
                                      Text(
                                        daysOfWeek[i],
                                        style: const TextStyle(
                                            color: Color(0xFF008B8f),
                                            fontSize: 17,
                                            fontWeight: FontWeight.w600),
                                      )
                                    else
                                      Text(
                                        daysOfWeek[i],
                                        style: const TextStyle(
                                            color: Colors.black54,
                                            fontSize: 17,
                                            fontWeight: FontWeight.w500),
                                      ),
                                const Spacer(),
                                IconButton(
                                  icon: const Icon(Icons.delete,
                                      color: Colors.black, size: 28),
                                  onPressed: () => _deleteAlarm(alarm, index),
                                ),
                                const Text('  '),
                              ],
                            ),
                          ],
                        ),
                      ),
                    ),
                  );
                },
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class Alarm {
  final TimeOfDay time;
  final String days;
  final String name;
  final int sound;
  final int level;
  final bool on;
  final int number;

  Alarm(
      {required this.sound,
      required this.level,
      required this.time,
      required this.name,
      required this.days,
      required this.on,
      required this.number});
}

class AlarmDataBase {
  List alarmList = [];
  List alarmAvail = List.filled(10, true);

  void createInitialData() {
    alarmAvail = List.filled(10, true);
    alarmList = [
      // ['07:00', '01234NN', 'College', 1, 1, true, 0],
      // ['09:00', 'NNNNN56', 'Weekend', 1, 1, true, 1],
    ];
  }

  final myBox = Hive.box('mybox');

  void loadData() {
    alarmList = myBox.get("ALARM");
    alarmAvail = myBox.get("AVAIL");
  }

  void updateDataBase() {
    myBox.put("ALARM", alarmList);
    myBox.put("AVAIL", alarmAvail);
  }
}
