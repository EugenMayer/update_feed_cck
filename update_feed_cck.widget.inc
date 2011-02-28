<?php
// $Id$

// Copyright (c) 2010 KontextWork
// Author: Eugen Mayer

/**
 * Implementation of hook_widget_info().
 */
function update_feed_cck_widget_info() {
  return array(
    'update_feed_cck_widget' => array(
      'field types' => array('update_feed_cck'),
      'label' => t('Update feed select dialog'),
      'multiple values' => CONTENT_HANDLE_CORE,
      'callbacks' => array(
          'default value' => CONTENT_CALLBACK_DEFAULT,
      ),
    ),
  );
}

function update_feed_cck_widget(&$form, &$form_state, $field, $items, $delta = 0) {
  $element= array(
    '#type' => 'update_feed_module',
    '#default_value' => $items[$delta],
  );
  return $element;
}

/**
 * An element #process callback for the update_feed_cck field type.
 *
 * Expands the filefield_widget type to include the upload field, upload and
 * remove buttons, and the description field.
 */
function update_feed_cck_update_feed_module_process($element, $edit, &$form_state, $form) {
  ctools_add_js('update_feed_cck','update_feed_cck');
  ctools_add_css('update_feed_cck','update_feed_cck');
  $defaults = $element['#value'];  
  $options = array();
  $feeds = _update_feed_api_get_feeds();
  foreach($feeds as $feed) {
    $options[$feed['server_key']] = $feed['server_key'];
  }
  $element['server_key'] = array(
    '#type' => 'select',
    '#title' => t('Server'),
    '#options' => $options,
    '#default_value' => $defaults['server_key'],
    '#size' => '1',
    '#attributes' => array (
      'class' => 'update_feed_cck_fieldi'
    ),
    '#prefix' => '<div class="update_feed_field_container">',
    '#suffix' => '</div>',
  );
  $types = _update_feed_api_package_types();  
  $element['ptype'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => drupal_map_assoc($types),
    '#default_value' => $defaults['ptype'],
    '#size' => 1,
    '#prefix' => '<div class="update_feed_field_container">',
    '#suffix' => '</div>',
    '#attributes' => array (
      'class' => 'update_feed_cck_field'
    ),
  );

  $element['short_name'] = array(
    '#type' => 'autocomplete',
    '#attributes' => array (
      'class' => 'update_feed_cck_field'
    ),
    '#title' => t('Module'),
    '#data' => 'update_feed_cck/autocomplete/modules',
    '#plugin_options' => array(
      'cacheLength' => 1,
      'mustMatch' => 1,
      'max' => 0,
      'extraParams' => array(
        'server_key' => array('update_feed_cck_field' => array('id' => 'select#'.$element['#id'].'-server-key') ),
        'ptype' => array('update_feed_cck_field' => array('id' => 'select#'.$element['#id'].'-ptype') ),
        'api' => array('update_feed_cck_field' => array('id' => 'select#'.$element['#id'].'-api-version') ),
      ),
    ),
    '#prefix' => '<div class="update_feed_field_container">',
    '#suffix' => '</div>',
    '#function_names' => array( 'update_feed_cck_server_key' ),
    '#default_value' => $defaults['short_name'],
    '#size' => '30',
  );
  $apis = _update_feed_api_allowed_apis();  
  $element['api_version'] = array(
    '#type' => 'select',
    '#title' => t('API'),
    '#options' => drupal_map_assoc($apis),
    '#default_value' => $defaults['api_version'],
    '#size' => 1,
    '#prefix' => '<div class="update_feed_field_container">',
    '#suffix' => '</div>',
    '#attributes' => array (
      'class' => 'update_feed_cck_field'
    ),
  );
  $releases = array();
  if(!empty($defaults['short_name']) && !empty($defaults['api_version'])) {
    $p = Project_factory::load_project($defaults['server_key'], $defaults['short_name'], $defaults['api_version'], TRUE, $defaults['ptype']);
    if($p == FALSE) {
      $element['server_key']['#attributes']['class'] .= ' error';
      $element['server_key']['#error'] = TRUE ;
      drupal_set_message('Could not load project data for "'. implode(', ', array($defaults['short_name'], $defaults['server_key'], $defaults['api_version']) ). '" - does the project exist?','warning');
    }
    else {
      $releases = $p->releases_to_array();      
    }
  }
  
  $version_not_found = FALSE;
  $has_newer = FALSE;
  // If the short name and module release is set, verify it exists
  if(!empty($defaults['short_name']) && !empty($defaults['module_release']) && array_search($defaults['module_release'],$releases) == FALSE) {    
    $version_not_found = TRUE;
    drupal_set_message('Could not find the current selected release '.$defaults['module_release'].' for package '.$defaults['short_name'],'warning');
    $releases = array_merge($releases, array($defaults['module_release']));
  }
  else if(is_object($p)){   
    $cur_release = $p->get_release($releases[$defaults['module_release']]);
    if($r = $cur_release->has_newer_release('mn')) {
     drupal_set_message($defaults['short_name'] . "({$defaults['module_release']}) has a newer minor release (".$r->get_info('version').')' );
     $has_newer = TRUE;
     $extra_class .= ' has_newer_version';
    }
  }
  if(empty($defaults['short_name'])) {
    $defaults['module_release'] = '';
  }
  $element['module_release'] = array(
    '#type' => 'select',
    '#title' => t('Release'),
    '#options' => drupal_map_assoc($releases),
    '#default_value' => $defaults['module_release'],
    '#size' => 2,
    '#attributes' => array (
      'class' => "update_feed_cck_field $extra_class"
    ),
    '#prefix' => '<div class="update_feed_field_container">',
    '#suffix' => $has_newer ? "<b>........*........</b></div>" : '</div>',
  );
  if($version_not_found) {
    $element['module_release']['#attributes']['class'] .= ' error';
  }
  
  $element['refresh'] = array(
    '#type' => 'button',
    '#title' => t('Get releases'),
    '#value' => t('Get releases'),
    '#ahah'   => array (
      'event'    => 'click',
      'method'   => 'replace',
      'wrapper'  =>  $element['#id'].'-module-release-wrapper',
      'path'     => 'update_feed_cck/autocomplete/releases/'.$form['#node']->type.'/'.$element['#field_name'].'/'.$element['#delta'],
      'effect'   => 'none',
      'progress' => array (
        'type'    => 'throbber',
        'message' => t('Refreshing'),
      ),
    ),
    '#prefix' => '<div class="update_feed_field_refresh clear-block">',
    '#suffix' => '</div>',   
  );
	$element['delete'] = array(
    '#type' => 'button',
    '#title' => t('Delete'),
    '#value' => t('Delete'),
    '#ahah'   => array (
      'event'    => 'click',
      'method'   => 'replace',
      'wrapper'  => str_replace('_','-',$element['#field_name']).'-items',
      'path'     => 'update_feed_cck/autocomplete/delete/'.$form['#node']->type.'/'.$element['#field_name'].'/'.$element['#delta'],
      'effect'   => 'none',
      'progress' => array (
        'type'    => 'throbber',
        'message' => t('Deleting'),
      ),
    ),
    '#prefix' => '<div class="update_feed_field_delete clear-block">',
    '#suffix' => '</div>',   
  );
  return $element;
}
