import 'package:app/alarms_screen.dart';
import 'package:flutter/material.dart';
import 'package:numberpicker/numberpicker.dart';

import 'time.dart';
import 'days.dart';
import 'name.dart';
import 'Ringtone.dart';
import 'diff.dart';

class TimePage extends StatefulWidget {
  final TimeOfDay initialTime;
  final String day;
  final String name;
  final int ringtone;
  final int level;
  final bool on;
  final int number;

  const TimePage(
      {required this.initialTime,
      required this.day,
      required this.name,
      required this.ringtone,
      required this.level,
      required this.on,
      required this.number});

  @override
  State<TimePage> createState() => _TimePageState();
}

class _TimePageState extends State<TimePage> {
  late TimeOfDay selectedTime;
  late String selectedDay;
  late String selectedName;
  late int selectedRingtone;
  late int selectedLevel;
  late bool selectedOn;
  late int selectedNumber;

  @override
  void initState() {
    super.initState();
    selectedTime = widget.initialTime;
    selectedDay = widget.day;
    selectedName = widget.name;
    selectedRingtone = widget.ringtone;
    selectedLevel = widget.level;
    selectedOn = widget.on;
    selectedNumber = widget.number;
  }

  void _saveTimeAndNavigateBack() {
    Navigator.pop(
        context,
        Alarm(
            time: selectedTime,
            days: selectedDay,
            name: selectedName,
            sound: selectedRingtone,
            level: selectedLevel,
            on: selectedOn,
            number: selectedNumber));
  }

  void handleTimeChanged(TimeOfDay newTime) {
    setState(() {
      selectedTime = newTime;
    });
  }

  void handleDayChanged(String newDay) {
    setState(() {
      selectedDay = newDay;
    });
  }

  void handleNameChanged(String newName) {
    setState(() {
      selectedName = newName;
    });
  }

  void handleRingtoneChanged(int newRingtone) {
    setState(() {
      selectedRingtone = newRingtone;
    });
  }

  void handleLevelChanged(int newLevel) {
    setState(() {
      selectedLevel = newLevel;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.grey[300],
      body: Padding(
        padding: const EdgeInsets.symmetric(horizontal: 10),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.start,
          children: [
            Text(
              "Time: ${selectedTime.hour.toString().padLeft(2, '0')}:${selectedTime.minute.toString().padLeft(2, "0")}",
              style: const TextStyle(fontWeight: FontWeight.bold, fontSize: 18),
            ),
            const SizedBox(
              height: 20,
            ),
            Container(
              padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 10),
              decoration: BoxDecoration(
                color: Colors.grey[300],
                borderRadius: BorderRadius.circular(10),
              ),
              child: TimePickerWidget(
                  selectedTime: selectedTime, onTimeChanged: handleTimeChanged),
            ),
            Container(
              padding: const EdgeInsets.symmetric(vertical: 5, horizontal: 5),
              decoration: BoxDecoration(
                color: Colors.grey[300],
                borderRadius: BorderRadius.circular(20),
              ),
              child: Column(
                children: [
                  DayPage(
                      selectedTime: selectedTime,
                      selectedDay: selectedDay,
                      onDayChanged: handleDayChanged),
                  const Divider(color: Colors.grey, thickness: 1.5, height: 25),
                  NameSelection(
                      selectedName: selectedName,
                      onNameChange: handleNameChanged),
                  const Divider(color: Colors.grey, thickness: 1.5, height: 25),
                  RingtoneSelection(
                      selectedRingtone: selectedRingtone,
                      onRingtoneChanged: handleRingtoneChanged),
                  const Divider(color: Colors.grey, thickness: 1.5, height: 25),
                  DifficultySelection(
                      selectedDifficulty: selectedLevel,
                      onDifficultyChanged: handleLevelChanged),
                ],
              ),
            ),
            const Spacer(),
            Row(
              mainAxisAlignment: MainAxisAlignment.spaceEvenly,
              children: [
                ElevatedButton(
                  onPressed: () {
                    Navigator.pop(context);
                  },
                  style: ButtonStyle(
                      elevation: MaterialStateProperty.all(0),
                      backgroundColor: MaterialStateProperty.all<Color>((Colors.grey[300]!))),
                  child: const Text('Cancel',
                      style: TextStyle(color: Color(0xFF008B8f), fontSize: 20)),
                ),
                ElevatedButton(
                  onPressed: _saveTimeAndNavigateBack,
                  style: ButtonStyle(
                      elevation: MaterialStateProperty.all(0),
                    backgroundColor: MaterialStateProperty.all<Color>((Colors.grey[300]!))),
                  child: const Text('Save',
                      style: TextStyle(color: Color(0xFF008B8f), fontSize: 20)),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}
