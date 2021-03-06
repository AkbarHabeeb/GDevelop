// @flow
import { Trans } from '@lingui/macro';

import * as React from 'react';
import TextField from 'material-ui/TextField';
import { Column, Spacer } from '../UI/Grid';
import SemiControlledTextField from '../UI/SemiControlledTextField';
import ObjectTypeSelector from '../ObjectTypeSelector';
import DismissableAlertMessage from '../UI/DismissableAlertMessage';
import AlertMessage from '../UI/AlertMessage';
const gd = global.gd;

type Props = {|
  project: gdProject,
  eventsFunctionsExtension: gdEventsFunctionsExtension,
  eventsBasedBehavior: gdEventsBasedBehavior,
|};

export default class EventsBasedBehaviorEditor extends React.Component<
  Props,
  {||}
> {
  // An array containing all the object types that are using the behavior
  _allObjectTypes: Array<string> = gd.WholeProjectRefactorer.getAllObjectTypesUsingEventsBasedBehavior(
    this.props.project,
    this.props.eventsFunctionsExtension,
    this.props.eventsBasedBehavior
  )
    .toNewVectorString()
    .toJSArray();

  render() {
    const { eventsBasedBehavior, project } = this.props;

    return (
      <Column>
        <DismissableAlertMessage
          identifier="events-based-behavior-explanation"
          kind="info"
        >
          This is the configuration of your behavior. Make sure to choose a
          proper internal name as it's hard to change it later. Enter a
          description explaining what the behavior is doing to the object.
        </DismissableAlertMessage>
        <TextField
          floatingLabelText={<Trans>Internal Name</Trans>}
          value={eventsBasedBehavior.getName()}
          disabled
          fullWidth
        />
        <SemiControlledTextField
          commitOnBlur
          floatingLabelText={<Trans>Name displayed in editor</Trans>}
          value={eventsBasedBehavior.getFullName()}
          onChange={text => {
            eventsBasedBehavior.setFullName(text);
            this.forceUpdate();
          }}
          fullWidth
        />
        <SemiControlledTextField
          commitOnBlur
          floatingLabelText={<Trans>Description</Trans>}
          floatingLabelFixed
          hintText={
            <Trans>
              The description of the behavior should explain what the behavior
              is doing to the object, and, briefly, how to use it.
            </Trans>
          }
          value={eventsBasedBehavior.getDescription()}
          onChange={text => {
            eventsBasedBehavior.setDescription(text);
            this.forceUpdate();
          }}
          multiLine
          fullWidth
          rows={3}
        />
        <ObjectTypeSelector
          floatingLabelText={
            <Trans>Object on which this behavior can be used</Trans>
          }
          project={project}
          value={eventsBasedBehavior.getObjectType()}
          onChange={(objectType: string) => {
            eventsBasedBehavior.setObjectType(objectType);
            this.forceUpdate();
          }}
          allowedObjectTypes={
            this._allObjectTypes.length === 0
              ? undefined /* Allow anything as the behavior is not used */
              : this._allObjectTypes.length === 1
              ? [
                  '',
                  this._allObjectTypes[0],
                ] /* Allow only the type of the objects using the behavior */
              : [
                  '',
                ] /* More than one type of object are using the behavior. Only "any object" can be used on this behavior */
          }
        />
        {this._allObjectTypes.length > 1 && (
          <AlertMessage kind="info">
            <Trans>
              This behavior is being used by multiple types of objects. Thus,
              you can't restrict its usage to any particular object type. All
              the object types using this behavior are listed here:
              {this._allObjectTypes.join(', ')}
            </Trans>
          </AlertMessage>
        )}
        {eventsBasedBehavior.getEventsFunctions().getEventsFunctionsCount() ===
          0 && (
          <DismissableAlertMessage
            identifier="empty-events-based-behavior-explanation"
            kind="info"
          >
            <Trans>
              Once you're done, close this dialog and start adding some
              functions to the behavior. Then, test the behavior by adding it to
              an object in a scene.
            </Trans>
          </DismissableAlertMessage>
        )}
        <Spacer />
      </Column>
    );
  }
}
